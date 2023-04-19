package com.ve.myandroidremotecontrol;

import static com.ve.myandroidremotecontrol.VETool.BytesToInteger;
import static com.ve.myandroidremotecontrol.VETool.IntegerToBytes;
import static com.ve.myandroidremotecontrol.VETool.MakeBytesCombine;

class CHeader {
    private int type;
    private int length;

    CHeader() {
    }

    CHeader(byte[] buffer) {
        type = BytesToInteger(buffer, 0);
        length = BytesToInteger(buffer, 4);
    }

    public byte[] getBytes() {
        return MakeBytesCombine(new byte[][]{IntegerToBytes(type), IntegerToBytes(length)});
    }

    public int getType() {
        return type;
    }

    public int getLength() {
        return length;
    }

    public void setType(int type) {
        this.type = type;
    }

    public void setLength(int length) {
        this.length = length;
    }
}
