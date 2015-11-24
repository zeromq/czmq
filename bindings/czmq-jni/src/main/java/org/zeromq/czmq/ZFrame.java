package org.zeromq.czmq;

public class ZFrame implements AutoCloseable {
    static {
        try {
            System.loadLibrary("czmqjni");
        } catch (Exception e) {
            System.exit(-1);
        }
    }
    long pointer;

    public ZFrame() {
        pointer = ZFrame.__init();
    }

    public ZFrame(long address) {
        pointer = address;
    }

    public ZFrame(byte[] buf) {
        this(buf, buf.length);
    }

    public ZFrame(byte[] buf, long size) {
        pointer = ZFrame.__init(buf, size);
    }

    public long size() {
        return ZFrame.__size(pointer);
    }

    public byte[] data() {
        return ZFrame.__data(pointer);
    }
    native static long __init();

    native static long __init(byte[] buf, long size);

    native static void __destroy(long pointer);

    native static long __size(long pointer);

    native static byte[] __data(long pointer);

    @Override
    public void close() {
        ZFrame.__destroy(pointer);
        pointer = 0;
    }
}
