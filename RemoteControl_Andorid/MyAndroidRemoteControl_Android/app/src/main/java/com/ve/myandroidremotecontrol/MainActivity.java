package com.ve.myandroidremotecontrol;

import androidx.activity.result.ActivityResult;
import androidx.activity.result.ActivityResultCallback;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.core.content.PermissionChecker;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.hardware.display.VirtualDisplay;
import android.media.ImageReader;
import android.media.projection.MediaProjection;
import android.media.projection.MediaProjectionManager;
import android.os.Build;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.security.Provider;


public class MainActivity extends AppCompatActivity {

    private TextView ipInput;
    private TextView portInput;
    private Button button;
    private Button button2;

    private String[] mPermissions;
    private int mScreenWidth;
    private int mScreenHeight;
    private int mScreenDensity;

    private MediaProjectionManager mMediaProjectionManager;
    private MediaProjection mMediaProjection;
    private ImageReader mImageReader;
    private VirtualDisplay mVirtualDisplay;
    private SocketThread mThread;
    private ActivityResultLauncher mLauncher;

    static final int RECORD_PERMISSION_CODE = 100;

//    @Override
//    protected void onDestroy() {
//        super.onDestroy();
//        stopService(new Intent(MainActivity.this, ScreenRecordService.class));
//    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        for (int i = 0; i < permissions.length; i++) {
            if (grantResults[i] != PermissionChecker.PERMISSION_GRANTED) {
                Toast.makeText(getApplicationContext(), permissions[i] + "  is Denied", Toast.LENGTH_SHORT).show();
            }
        }

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 控件绑定
        ipInput = findViewById(R.id.editTextTextPersonName);
        portInput = findViewById(R.id.editTextTextPersonName2);
        button = findViewById(R.id.button);
        button2 = findViewById(R.id.button2);

        // 设定控件默认显示
        ipInput.setText("169.254.78.232");
        portInput.setText("9527");

        // 请求所需权限
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            ActivityCompat.requestPermissions(this, mPermissions = new String[]{
                    Manifest.permission.INTERNET,
                    Manifest.permission.RECORD_AUDIO,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.READ_EXTERNAL_STORAGE,
            }, RECORD_PERMISSION_CODE);
        }

        // 注册Connect点击事件响应
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                for (int i = 0; i < mPermissions.length; i++) {
                    if (ContextCompat.checkSelfPermission(v.getContext(), mPermissions[i]) != PermissionChecker.PERMISSION_GRANTED) {
                        Toast.makeText(v.getContext(), "No permission --> " + mPermissions[i], Toast.LENGTH_SHORT).show();
                        return;
                    }
                }

                mMediaProjectionManager = (MediaProjectionManager) getSystemService(Context.MEDIA_PROJECTION_SERVICE);
                Intent intent = mMediaProjectionManager.createScreenCaptureIntent();
                mLauncher.launch(intent);
            }
        });

        // 注册DisConnect点击事件响应
        button2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                stopService(new Intent(MainActivity.this, ScreenRecordService.class));
            }
        });

        // 注册Activity回调
        mLauncher = registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), new ActivityResultCallback<ActivityResult>() {
            @Override
            public void onActivityResult(ActivityResult result) {
                if (result.getResultCode() == RESULT_OK) {
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {

                        String host = ipInput.getText().toString();
                        String port = portInput.getText().toString();
                        DisplayMetrics metrics = new DisplayMetrics();
                        getWindowManager().getDefaultDisplay().getMetrics(metrics);
                        mScreenWidth = metrics.widthPixels;
                        mScreenHeight = metrics.heightPixels;
                        mScreenDensity = metrics.densityDpi;

                        Intent intent = new Intent(MainActivity.this, ScreenRecordService.class);
                        intent.putExtra("resultCode", result.getResultCode());
                        intent.putExtra("data", result.getData());
                        intent.putExtra("width", mScreenWidth);
                        intent.putExtra("height", mScreenHeight);
                        intent.putExtra("density", mScreenDensity);
                        intent.putExtra("dest_ip", host);
                        intent.putExtra("dest_port", port);
                        startForegroundService(intent);
                    }
                }
            }
        });
    }


}