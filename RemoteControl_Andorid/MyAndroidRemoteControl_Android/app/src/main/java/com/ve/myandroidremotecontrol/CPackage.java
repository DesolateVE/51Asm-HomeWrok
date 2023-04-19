package com.ve.myandroidremotecontrol;

public class CPackage {
    private CHeader header;
    private byte[] body;

    CPackage(byte[] buffer) {
        header = new CHeader(buffer);
        body = new byte[header.getLength()];
        System.arraycopy(buffer, 8, body, 0, body.length);
    }

    public byte[] getBody() {
        return body;
    }

    public CHeader getHeader() {
        return header;
    }

}
