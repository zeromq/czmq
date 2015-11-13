package org.zeromq.czmq;

/**
 * The zctx class wraps ØMQ contexts. It manages open sockets in the context and automatically closes these before terminating the context. It provides a simple way to set the linger timeout on sockets, and configure contexts for number of I/O threads. Sets-up signal (interrupt) handling for the process.
 */
public final class ZCtx implements AutoCloseable {
    long pointer;

    public ZCtx() {
        pointer = ZCtx.__init();
    }

    native static long __init();

    native static void __destroy(long pointer);

    native static void __set_iothreads(long pointer, int threads);

    native static void __set_linger(long pointer, int linger);

    public void setIoThreads(int ioThreads) {
        ZCtx.__set_iothreads(pointer, ioThreads);
    }

    public void setLinger(int linger) {
        ZCtx.__set_linger(pointer, linger);
    }

    @Override
    public void close() throws Exception {
        ZCtx.__destroy(pointer);
    }
}
