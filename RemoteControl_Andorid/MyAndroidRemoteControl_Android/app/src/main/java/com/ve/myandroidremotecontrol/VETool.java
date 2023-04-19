package com.ve.myandroidremotecontrol;

import android.app.Activity;
import android.util.DisplayMetrics;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

public class VETool {

    static byte[] IntegerToBytes(int num) {
        byte[] buffer = new byte[4];
        buffer[0] = (byte) (num & 0xff);
        buffer[1] = (byte) (num >>> 8 & 0xff);
        buffer[2] = (byte) (num >>> 16 & 0xff);
        buffer[3] = (byte) (num >>> 24 & 0xff);
        return buffer;
    }

//    static byte[] MakeBytesCombine(byte[][] list) {
//        int length = 0;
//        int p = 0;
//        for (byte[] var : list) {
//            length += var.length;
//        }
//        byte[] buffer = new byte[length];
//        for (byte[] var : list) {
//            for (byte b : var) {
//                buffer[p++] = b;
//            }
//        }
//        return buffer;
//    }

    // Java 数据传输默认使用大端，order设定只对读取有用，所以这里是无效的
    // 转换可参考 DataOutputStream
//    public static byte[] IntegerToBytes(int num) {
//        return ByteBuffer.allocate(4).putInt(num).order(ByteOrder.LITTLE_ENDIAN).array();
//    }

    static int BytesToInteger(byte[] buffer, int offset) {
        return ByteBuffer.wrap(buffer, offset, 4).order(ByteOrder.LITTLE_ENDIAN).getInt();
    }

    static byte[] MakeBytesCombine(byte[][] list) {
        int length = 0;
        int p = 0;
        for (byte[] var : list) {
            length += var.length;
        }
        byte[] buffer = new byte[length];
        for (byte[] var : list) {
            System.arraycopy(var, 0, buffer, p, var.length);
            p += var.length;
        }
        return buffer;
    }

}
