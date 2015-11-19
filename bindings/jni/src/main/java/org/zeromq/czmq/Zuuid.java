package org.zeromq.czmq;

public class Zuuid implements AutoCloseable {
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
    public Zuuid (long address) {
        pointer = address;
    }

    native static void __print (long pointer);
    public void print (long pointer) {
        return Zuuid.__print (pointer);
    }

    native static Zuuid __new_from (byte [] source);
    public Zuuid new_from (byte [] source) {
        return Zuuid.__new_from (source);
    }

    native static void __set (long pointer,byte [] source);
    public void set (long pointer,byte [] source) {
        return Zuuid.__set (pointer,source);
    }

    native static int __set_str (long pointer,String source);
    public int set_str (long pointer,String source) {
        return Zuuid.__set_str (pointer,source);
    }

    native static byte [] __data (long pointer);
    public byte [] data (long pointer) {
        return Zuuid.__data (pointer);
    }

    native static long __size (long pointer);
    public long size (long pointer) {
        return Zuuid.__size (pointer);
    }

    native static String __str (long pointer);
    public String str (long pointer) {
        return Zuuid.__str (pointer);
    }

    native static String __str_canonical (long pointer);
    public String str_canonical (long pointer) {
        return Zuuid.__str_canonical (pointer);
    }

    native static void __export (long pointer,byte [] target);
    public void export (long pointer,byte [] target) {
        return Zuuid.__export (pointer,target);
    }

    native static boolean __eq (long pointer,byte [] compare);
    public boolean eq (long pointer,byte [] compare) {
        return Zuuid.__eq (pointer,compare);
    }

    native static boolean __neq (long pointer,byte [] compare);
    public boolean neq (long pointer,byte [] compare) {
        return Zuuid.__neq (pointer,compare);
    }

    native static Zuuid __dup (long pointer);
    public Zuuid dup (long pointer) {
        return Zuuid.__dup (pointer);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Zuuid.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
