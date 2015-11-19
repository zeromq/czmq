package org.zeromq.czmq;

public class Ziflist implements AutoCloseable {
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
    public Ziflist (long address) {
        pointer = address;
    }

    native static void __print (long pointer);
    public void print (long pointer) {
        return Ziflist.__print (pointer);
    }

    native static void __reload (long pointer);
    public void reload (long pointer) {
        return Ziflist.__reload (pointer);
    }

    native static long __size (long pointer);
    public long size (long pointer) {
        return Ziflist.__size (pointer);
    }

    native static String __first (long pointer);
    public String first (long pointer) {
        return Ziflist.__first (pointer);
    }

    native static String __next (long pointer);
    public String next (long pointer) {
        return Ziflist.__next (pointer);
    }

    native static String __address (long pointer);
    public String address (long pointer) {
        return Ziflist.__address (pointer);
    }

    native static String __broadcast (long pointer);
    public String broadcast (long pointer) {
        return Ziflist.__broadcast (pointer);
    }

    native static String __netmask (long pointer);
    public String netmask (long pointer) {
        return Ziflist.__netmask (pointer);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Ziflist.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
