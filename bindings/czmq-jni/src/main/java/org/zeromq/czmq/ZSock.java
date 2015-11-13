package org.zeromq.czmq;

/**
 * ZeroMQ sockets present an abstraction of an asynchronous message queue, with the exact queuing
 * semantics depending on the socket type in use. Where conventional sockets transfer streams of
 * bytes or discrete datagrams, ZeroMQ sockets transfer discrete messages.
 * <p>
 * ZeroMQ sockets being asynchronous means that the timings of the physical connection setup and
 * tear down, reconnect and effective delivery are transparent to the user and organized by ZeroMQ
 * itself. Further, messages may be queued in the event that a peer is unavailable to receive them.
 * </p>
 */
public class ZSock implements AutoCloseable {
    static {
        try {
            System.loadLibrary("czmqjni");
        } catch (Exception e) {
            System.exit(-1);
        }
    }

    long pointer;

    public ZSock(int type) {
        pointer = ZSock.__init(type);
    }

    native static long __init(int type);

    native static void __destroy(long pointer);

    native static int __bind(long pointer, String endpoint);

    native static int __unbind(long pointer, String endpoint);

    native static int __connect(long pointer, String endpoint);

    native static int __disconnect(long pointer, String endpoint);

    public boolean bind(String endpoint) {
        final int rc = __bind(pointer, endpoint);
        return rc == 0;
    }

    public boolean unbind(String endpoint) {
        final int rc = __unbind(pointer, endpoint);
        return rc == 0;
    }

    public boolean connect(String endpoint) {
        final int rc = __connect(pointer, endpoint);
        return rc == 0;
    }

    public boolean disconnect(String endpoint) {
        final int rc = __disconnect(pointer, endpoint);
        return rc == 0;
    }

    @Override
    public void close() {
        ZSock.__destroy(pointer);
    }
}
