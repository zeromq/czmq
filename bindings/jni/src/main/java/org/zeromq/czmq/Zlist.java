package org.zeromq.czmq;

public class Zlist implements AutoCloseable {
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
    public Zlist (long address) {
        pointer = address;
    }

    native static void * __first (long pointer);
    public void * first (long pointer) {
        return Zlist.__first (pointer);
    }

    native static void * __next (long pointer);
    public void * next (long pointer) {
        return Zlist.__next (pointer);
    }

    native static void * __last (long pointer);
    public void * last (long pointer) {
        return Zlist.__last (pointer);
    }

    native static void * __head (long pointer);
    public void * head (long pointer) {
        return Zlist.__head (pointer);
    }

    native static void * __tail (long pointer);
    public void * tail (long pointer) {
        return Zlist.__tail (pointer);
    }

    native static void * __item (long pointer);
    public void * item (long pointer) {
        return Zlist.__item (pointer);
    }

    native static int __append (long pointer,void * item);
    public int append (long pointer,void * item) {
        return Zlist.__append (pointer,item);
    }

    native static int __push (long pointer,void * item);
    public int push (long pointer,void * item) {
        return Zlist.__push (pointer,item);
    }

    native static void * __pop (long pointer);
    public void * pop (long pointer) {
        return Zlist.__pop (pointer);
    }

    native static boolean __exists (long pointer,void * item);
    public boolean exists (long pointer,void * item) {
        return Zlist.__exists (pointer,item);
    }

    native static void __remove (long pointer,void * item);
    public void remove (long pointer,void * item) {
        return Zlist.__remove (pointer,item);
    }

    native static Zlist __dup (long pointer);
    public Zlist dup (long pointer) {
        return Zlist.__dup (pointer);
    }

    native static void __purge (long pointer);
    public void purge (long pointer) {
        return Zlist.__purge (pointer);
    }

    native static long __size (long pointer);
    public long size (long pointer) {
        return Zlist.__size (pointer);
    }

    native static void __sort (long pointer);
    public void sort (long pointer) {
        return Zlist.__sort (pointer);
    }

    native static void __autofree (long pointer);
    public void autofree (long pointer) {
        return Zlist.__autofree (pointer);
    }

    native static void __comparefn (long pointer);
    public void comparefn (long pointer) {
        return Zlist.__comparefn (pointer);
    }

    native static void * __freefn (long pointer,void * item, boolean atTail);
    public void * freefn (long pointer,void * item, boolean atTail) {
        return Zlist.__freefn (pointer,item, atTail);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Zlist.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
