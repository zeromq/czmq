package org.zeromq.czmq;

public class ZMQ {
    /* Context options */
    public static final int ZMQ_IO_THREADS = 1;
    public static final int ZMQ_MAX_SOCKETS = 2;
    /* Default for new contexts */
    public static final int ZMQ_IO_THREADS_DFLT = 1;
    public static final int ZMQ_MAX_SOCKETS_DFLT = 1024;
    /* Socket types. */
    public static final int ZMQ_PAIR = 0;
    public static final int ZMQ_PUB = 1;
    public static final int ZMQ_SUB = 2;
    public static final int ZMQ_REQ = 3;
    public static final int ZMQ_REP = 4;
    public static final int ZMQ_DEALER = 5;
    public static final int ZMQ_ROUTER = 6;
    public static final int ZMQ_PULL = 7;
    public static final int ZMQ_PUSH = 8;
    public static final int ZMQ_XPUB = 9;
    public static final int ZMQ_XSUB = 10;
    /* Deprecated aliases */
    @Deprecated
    public static final int ZMQ_XREQ = ZMQ_DEALER;
    @Deprecated
    public static final int ZMQ_XREP = ZMQ_ROUTER;

    private ZMQ() {
    }
}
