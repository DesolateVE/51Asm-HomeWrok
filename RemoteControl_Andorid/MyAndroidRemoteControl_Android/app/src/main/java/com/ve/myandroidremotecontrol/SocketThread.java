package com.ve.myandroidremotecontrol;

import static com.ve.myandroidremotecontrol.VETool.IntegerToBytes;
import static com.ve.myandroidremotecontrol.VETool.MakeBytesCombine;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.Image;
import android.media.ImageReader;
import android.media.projection.MediaProjection;
import android.util.Log;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class SocketThread extends Thread {

    private String host;
    private String port;
    private Socket socket;
    private InputStream socket_inStream;
    private OutputStream socket_outStream;
    private BlockingQueue<CPackage> queue;

    private boolean global_switch;
    private boolean screen_switch;
    private boolean su_permission;

    private int ScreenWidth;
    private int ScreenHeight;
    private int ScreenDensity;

    private MediaProjection mediaProjection;
    private ImageReader imageReader;
    private VirtualDisplay virtualDisplay;
    private VECallBack stopServiceCallBack;

    private Process process;
    private InputStream cmd_inStream;
    private InputStream cmd_errorStream;
    private OutputStream cmd_outStream;

    static final int PACKAGE_HEADER_SIZE = 8;
    static final int REQUEST_WINDOW_SHOW_ON = 100;
    static final int REQUEST_WINDOW_SHOW_OFF = 110;
    static final int REQUEST_ROOT_PERMISSION = 120;
    static final int STREAM_WINDOW_SHOW = 200;
    static final int STREAM_INPUT_CTRL = 210;
    static final int STREAM_SHELL_DATA = 220;
    static final int SOCKET_DISCONNECT = 300;

    SocketThread(String host, String port) {
        this.host = host;
        this.port = port;
    }

    protected void OnWindowShow() {
        try {
            // 返回屏幕信息
            CHeader header = new CHeader();
            header.setType(REQUEST_WINDOW_SHOW_ON);
            header.setLength(8);
            byte[] temp1 = IntegerToBytes(ScreenWidth);
            byte[] temp2 = IntegerToBytes(ScreenHeight);
            byte[] buffer = MakeBytesCombine(new byte[][]{header.getBytes(), temp1, temp2});
            socket_outStream.write(buffer);

            // 开启画面传输线程
            new Thread() {
                @Override
                public void run() {
                    super.run();

                    screen_switch = true;
                    WindowCaptureStart();
                    Bitmap bitmap = Bitmap.createBitmap(ScreenWidth, ScreenHeight, Bitmap.Config.ARGB_8888);

                    while (screen_switch) {
                        try {
                            Image image = imageReader.acquireLatestImage();
                            if (image == null) {
                                continue;
                            }
                            final Image.Plane[] planes = image.getPlanes();
                            final ByteBuffer buffer = planes[0].getBuffer();

                            bitmap.copyPixelsFromBuffer(buffer);
                            ByteArrayOutputStream byteStream = new ByteArrayOutputStream();
                            bitmap.compress(Bitmap.CompressFormat.PNG, 100, byteStream);

                            header.setType(STREAM_WINDOW_SHOW);
                            header.setLength(byteStream.size());

                            socket_outStream.write(header.getBytes());
                            socket_outStream.write(byteStream.toByteArray());
                            image.close();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }
            }.start();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    protected void UnPack() {
        new Thread() {
            @Override
            public void run() {
                super.run();
                while (global_switch) {
                    try {
                        CPackage cPackage = queue.take();
                        if (cPackage == null) {
                            continue;
                        }
                        //Log.d("VELog", "PackageType-->" + cPackage.getHeader().getType() + "\tPackageLength-->" + cPackage.getHeader().getLength());
                        //Log.d("VELog", "Message-->" + new String(cPackage.getBody()));
                        switch (cPackage.getHeader().getType()) {
                            case REQUEST_WINDOW_SHOW_ON:    // 开启投屏，首先返回设备屏幕信息，然后传输屏幕数据流
                                OnWindowShow();
                                break;
                            case REQUEST_WINDOW_SHOW_OFF:
                                screen_switch = false;
                                break;
                            case REQUEST_ROOT_PERMISSION:
                                OnSuWanted();
                                break;
                            case STREAM_INPUT_CTRL:         // 按键映射流
                                OnKeyInput(cPackage.getBody());
                                break;
                            case STREAM_SHELL_DATA:
                                OnShellCommand(cPackage.getBody());
                                break;
                            case SOCKET_DISCONNECT:
                                SafeClosed();
                                break;
                        }
                    } catch (InterruptedException | IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }.start();
    }

    private void OnShellCommand(byte[] cmd) throws IOException, InterruptedException {
        if (su_permission) {
            cmd_outStream.write(new String(cmd).getBytes());
            cmd_outStream.flush();

            sleep(200);

            int length;
            CHeader header = new CHeader();
            header.setType(STREAM_SHELL_DATA);
            if ((length = cmd_errorStream.available()) != 0) {
                header.setLength(length);
                byte[] buffer = new byte[length];
                cmd_errorStream.read(buffer, 0, length);
                socket_outStream.write(header.getBytes());
                socket_outStream.write(buffer);
            }
            if ((length = cmd_inStream.available()) != 0) {
                header.setLength(length);
                byte[] buffer = new byte[length];
                cmd_inStream.read(buffer, 0, length);
                socket_outStream.write(header.getBytes());
                socket_outStream.write(buffer);
            }
        }
    }

    private void OnSuWanted() throws IOException {
        if (!su_permission) {
            process = Runtime.getRuntime().exec("su");
            if ((new File("/system/bin/su").exists()) &&
                    (new File("/system/xbin/su").exists())) {
                cmd_outStream = process.getOutputStream();
                cmd_inStream = process.getInputStream();
                cmd_errorStream = process.getErrorStream();
                su_permission = true;
            }
        }
    }

    private void OnKeyInput(byte[] cmd) throws IOException {
        if (su_permission) {
            cmd_outStream.write(new String(cmd).getBytes());
            cmd_outStream.flush();
        }
    }

    @Override
    public void run() {
        super.run();

        // 尝试建立链接
        try {
            socket = new Socket(host, Integer.parseInt(port));
            socket_inStream = socket.getInputStream();
            socket_outStream = socket.getOutputStream();
        } catch (IOException e) {
            e.printStackTrace();
            stopServiceCallBack.Fun();
            return;
        }

        // 打印日志链接成功
        //Log.d("VEDebug", "Connect Success");

        // 循环开关开启
        global_switch = true;

        // 包事件线程初始化
        queue = new LinkedBlockingQueue<CPackage>();
        UnPack();

        // 按拆包规则分发数据
        byte[] buffer = new byte[1 << 21];
        int length = 0;
        int n;
        while (global_switch) {
            try {
                if (length < PACKAGE_HEADER_SIZE) {
                    n = socket_inStream.read(buffer, length, buffer.length - length);
                    if (n != -1) {
                        length += n;
                    }
                    continue;
                }
                CHeader header = new CHeader(buffer);
                int size = header.getLength();
                if (length < PACKAGE_HEADER_SIZE + size) {
                    n = socket_inStream.read(buffer, length, buffer.length - length);
                    if (n != -1) {
                        length += n;
                    }
                    continue;
                }
                CPackage Package = new CPackage(buffer);
                queue.put(Package);
                length -= (PACKAGE_HEADER_SIZE + size);
                if (length > 0) {
                    System.arraycopy(buffer, PACKAGE_HEADER_SIZE + size, buffer, 0, length);
                }
            } catch (IOException e) {
                e.printStackTrace();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        // 打印日志断开连接
        //Log.d("VEDebug", "Host is disconnected");
        stopServiceCallBack.Fun();
    }

    @SuppressLint("WrongConstant")
    public void WindowCaptureStart() {
        imageReader = ImageReader.newInstance(ScreenWidth, ScreenHeight, PixelFormat.RGBA_8888, 3);
        virtualDisplay = mediaProjection.createVirtualDisplay("screen-mirror",
                ScreenWidth, ScreenHeight, ScreenDensity, DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR,
                imageReader.getSurface(), null, null);
    }

    public void WindowCaptureClose() {
        if (virtualDisplay == null) {
            return;
        }
        virtualDisplay.release();
        virtualDisplay = null;
    }

    public void SafeClosed() {
        try {
            screen_switch = false;
            global_switch = false;
            WindowCaptureClose();
            process.destroy();
            socket.close();
            socket = null;
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public Socket getSocket() {
        return socket;
    }

    public void setSocket(Socket socket) {
        this.socket = socket;
    }

    public int getScreenWidth() {
        return ScreenWidth;
    }

    public void setScreenWidth(int mScreenWidth) {
        this.ScreenWidth = mScreenWidth;
    }

    public int getScreenHeight() {
        return ScreenHeight;
    }

    public void setScreenHeight(int mScreenHeight) {
        this.ScreenHeight = mScreenHeight;
    }

    public int getScreenDensity() {
        return ScreenDensity;
    }

    public void setScreenDensity(int screenDensity) {
        ScreenDensity = screenDensity;
    }

    public MediaProjection getMediaProjection() {
        return mediaProjection;
    }

    public void setMediaProjection(MediaProjection mediaProjection) {
        this.mediaProjection = mediaProjection;
    }

    public void setOnStopServiceCallBack(VECallBack callBack) {
        stopServiceCallBack = callBack;
    }
}
