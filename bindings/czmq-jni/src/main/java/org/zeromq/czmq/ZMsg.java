package org.zeromq.czmq;

/**
 * Working with multipart messages.
 * <p>
 * This class provides a list-like container interface, with methods to work with the overall
 * container. Messages are composed of zero or more Frames.
 * </p>
 */
public class ZMsg implements AutoCloseable {

    static {
        try {
            System.loadLibrary("czmqjni");
        } catch (Exception e) {
            System.exit(-1);
        }
    }

    long pointer; // private-package

    /**
     * Constructs an empty message
     */
    public ZMsg() {
        pointer = ZMsg.__init();
    }

    ZMsg(long pointer) { // private-package
        this.pointer = pointer;
    }

    native static long __init();

    native static void __destroy(long pointer);

    native static int __size(long pointer);

    native static long __content_size(long pointer);

    native static long __recv(long pointer);

    native static long __send(long pointer, long dest);

    native static int __pushstr(long pointer, String str);

    native static String __popstr(long pointer);

    public static ZMsg recv(ZSock sock) {
        final long ptr = ZMsg.__recv(sock.pointer);
        return new ZMsg(ptr);
    }

    public static long send(ZMsg msg, ZSock dest) {
        return ZMsg.__send(msg.pointer, dest.pointer);
    }

    /**
     * Return size of message, i.e. number of frames (0 or more).
     */
    public int size() {
        return ZMsg.__size(pointer);
    }

    /**
     * Return total size of all frames in message.
     */
    public long contentSize() {
        return ZMsg.__content_size(pointer);
    }

    public boolean prepend(ZFrame frame) {
        return true;
    }

    public boolean append(ZFrame frame) {
        return true;
    }

    public ZFrame pop() {
        return null;
    }

    public boolean pushstr(String str) {
        return 0 == __pushstr(pointer, str);
    }

    public boolean pushstrf(String format, Object... args) {
        return true;
    }

    public ZMsg popmsg() {
        return null;
    }

    public boolean addstrf(String format, Object... args) {
        return true;
    }

    public String popstr() {
        return __popstr(pointer);
    }

    @Override
    public void close() {
        ZMsg.__destroy(pointer);
    }
}
