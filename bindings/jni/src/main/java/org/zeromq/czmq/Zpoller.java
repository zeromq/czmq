package org.zeromq.czmq;

public class Zpoller implements AutoCloseable {
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
    public Zpoller (long address) {
        pointer = address;
    }

    native static int __add (long pointer,void * reader);
    public int add (long pointer,void * reader) {
        return Zpoller.__add (pointer,reader);
    }

    native static int __remove (long pointer,void * reader);
    public int remove (long pointer,void * reader) {
        return Zpoller.__remove (pointer,reader);
    }

    native static void * __wait (long pointer,int timeout);
    public void * wait (long pointer,int timeout) {
        return Zpoller.__wait (pointer,timeout);
    }

    native static boolean __expired (long pointer);
    public boolean expired (long pointer) {
        return Zpoller.__expired (pointer);
    }

    native static boolean __terminated (long pointer);
    public boolean terminated (long pointer) {
        return Zpoller.__terminated (pointer);
    }

    native static void __ignore_interrupts (long pointer);
    public void ignore_interrupts (long pointer) {
        return Zpoller.__ignore_interrupts (pointer);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Zpoller.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
