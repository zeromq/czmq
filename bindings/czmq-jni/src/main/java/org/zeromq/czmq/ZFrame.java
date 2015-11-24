package org.zeromq.czmq;

public class ZFrame implements AutoCloseable {

    long pointer;

    public ZFrame() {
        pointer = ZFrame.__init();
    }

    public ZFrame(long address) {
        pointer = address;
    }

    public ZFrame(byte[] buf, long size) {
        pointer = ZFrame.__init(buf, size);
    }

    public long size() {
        return ZFrame.__size(pointer);
    }
    native static long __init();

    native static long __init(byte[] buf, long size);

    native static void __destroy(long pointer);

    native static long __size(long pointer);

    @Override
    public void close() {
        ZFrame.__destroy(pointer);
        pointer = 0;
    }
}
