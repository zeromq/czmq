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

    public ZMsg(long pointer) {
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

    native static long __pop(long pointer);

    native static int __prepend(long pointer, long zframePtr);

    native static int __append(long pointer, long zframePtr);

    public static ZMsg recv(ZSock sock) {
        final long ptr = ZMsg.__recv(sock.pointer);
        return new ZMsg(ptr);
    }

    public static long send(ZMsg msg, ZSock dest) {
        return ZMsg.__send(msg.pointer, dest.pointer);
    }

    public long getAddress() {
        return pointer;
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
        return 0 == ZMsg.__prepend(pointer, frame.pointer);
    }

    public boolean append(ZFrame frame) {
        return 0 == ZMsg.__append(pointer, frame.pointer);
    }

    public ZFrame pop() {
        final long zframePtr = ZMsg.__pop(pointer);
        return new ZFrame(zframePtr);
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
