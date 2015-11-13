package org.zeromq.czmq;

public class ZFrame implements AutoCloseable {

    long pointer;

    public ZFrame() {
        pointer = __init();
    }

    native static long __init();

    native static long __init(byte[] buf, long size);

    native static void __destroy(long pointer);

    @Override
    public void close() throws Exception {
        __destroy(pointer);
        pointer = 0;

    }
}
