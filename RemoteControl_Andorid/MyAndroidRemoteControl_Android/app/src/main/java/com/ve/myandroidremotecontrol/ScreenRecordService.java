package com.ve.myandroidremotecontrol;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.media.projection.MediaProjection;
import android.media.projection.MediaProjectionManager;
import android.os.Binder;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.util.DisplayMetrics;

import androidx.core.app.NotificationCompat;

import javax.security.auth.callback.Callback;


public class ScreenRecordService extends Service {

    private MediaProjectionManager mMediaProjectionManager;
    private MediaProjection mMediaProjection;

    private int mScreenWidth;
    private int mScreenHeight;
    private int mScreenDensity;
    private SocketThread mSocketThread;

    public class ScreenRecordBinder extends Binder {
        public ScreenRecordService getScreenRecordService() {
            return ScreenRecordService.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return new ScreenRecordBinder();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        Bundle bundle = intent.getExtras();
        mScreenWidth = (int) bundle.get("width");
        mScreenHeight = (int) bundle.get("height");
        mScreenDensity = (int) bundle.get("density");
        int result_code = (int) bundle.get("resultCode");
        Intent result_data = (Intent) bundle.get("data");
        String dest_ip = bundle.getString("dest_ip");
        String dest_port = bundle.getString("dest_port");
        mMediaProjectionManager = (MediaProjectionManager) getSystemService(Context.MEDIA_PROJECTION_SERVICE);

        String CHANNEL_ONE_ID = "com.ve_application.cn";
        String CHANNEL_ONE_NAME = "Channel One";
        NotificationChannel notificationChannel = null;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            notificationChannel = new NotificationChannel(CHANNEL_ONE_ID,
                    CHANNEL_ONE_NAME, NotificationManager.IMPORTANCE_HIGH);
            notificationChannel.enableLights(true);
            notificationChannel.setLightColor(Color.RED);
            notificationChannel.setShowBadge(true);
            notificationChannel.setLockscreenVisibility(Notification.VISIBILITY_PUBLIC);
            NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
            manager.createNotificationChannel(notificationChannel);
        }

        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, intent, PendingIntent.FLAG_MUTABLE);
        Notification notification = new NotificationCompat.Builder(this, CHANNEL_ONE_ID)
                .setTicker("Nature")
                .setSmallIcon(R.mipmap.ic_launcher)
                .setContentTitle("Remote Control")
                .setContentIntent(pendingIntent).build();
        notification.flags |= Notification.FLAG_NO_CLEAR;

        startForeground(9527, notification);

        mMediaProjection = mMediaProjectionManager.getMediaProjection(result_code, result_data);


        // 初始化 socket 收发包线程
        mSocketThread = new SocketThread(dest_ip, dest_port);
        mSocketThread.setScreenWidth(mScreenWidth);
        mSocketThread.setScreenHeight(mScreenHeight);
        mSocketThread.setScreenDensity(mScreenDensity);
        mSocketThread.setMediaProjection(mMediaProjection);
        mSocketThread.setOnStopServiceCallBack(new VECallBack() {
            @Override
            public void Fun() {
                stopSelf();
            }
        });
        mSocketThread.start();

        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        if (mSocketThread.getSocket() != null) {
            mSocketThread.SafeClosed();
        }
        if (mMediaProjection != null) {
            mMediaProjection.stop();
            mMediaProjection = null;
        }
    }


}
