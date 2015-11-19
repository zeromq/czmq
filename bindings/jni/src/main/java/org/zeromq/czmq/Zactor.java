package org.zeromq.czmq;

public class Zactor implements AutoCloseable {
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
    public Zactor (long address) {
        pointer = address;
    }

    native static int __send (long pointer,Zmsg msgP);
    public int send (long pointer,Zmsg msgP) {
        return Zactor.__send (pointer,msgP);
    }

    native static Zmsg __recv (long pointer);
    public Zmsg recv (long pointer) {
        return Zactor.__recv (pointer);
    }

    native static boolean __is (void * self);
    public boolean is (void * self) {
        return Zactor.__is (self);
    }

    native static void * __resolve (void * self);
    public void * resolve (void * self) {
        return Zactor.__resolve (self);
    }

    native static Zsock __sock (long pointer);
    public Zsock sock (long pointer) {
        return Zactor.__sock (pointer);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Zactor.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
