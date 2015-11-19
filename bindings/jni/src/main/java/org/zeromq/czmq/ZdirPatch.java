package org.zeromq.czmq;

public class ZdirPatch implements AutoCloseable {
    static {
        try {
            System.loadLibrary ("czmqjni");
        }
        catch (Exception e) {
            System.exit (-1);
        }
    }
    long pointer;

    native static long __init ();
    public () {
        pointer = __init ();
    }
    public ZdirPatch (long address) {
        pointer = address;
    }

    native static ZdirPatch __dup (long pointer);
    public ZdirPatch dup (long pointer) {
        return ZdirPatch.__dup (pointer);
    }

    native static String __path (long pointer);
    public String path (long pointer) {
        return ZdirPatch.__path (pointer);
    }

    native static Zfile __file (long pointer);
    public Zfile file (long pointer) {
        return ZdirPatch.__file (pointer);
    }

    native static void __op (long pointer);
    public void op (long pointer) {
        return ZdirPatch.__op (pointer);
    }

    native static String __vpath (long pointer);
    public String vpath (long pointer) {
        return ZdirPatch.__vpath (pointer);
    }

    native static void __digest_set (long pointer);
    public void digest_set (long pointer) {
        return ZdirPatch.__digest_set (pointer);
    }

    native static String __digest (long pointer);
    public String digest (long pointer) {
        return ZdirPatch.__digest (pointer);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return ZdirPatch.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
