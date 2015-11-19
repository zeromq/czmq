package org.zeromq.czmq;

public class Zframe implements AutoCloseable {
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
    public Zframe (long address) {
        pointer = address;
    }

    native static Zframe __new_empty ();
    public Zframe new_empty () {
        return Zframe.__new_empty ();
    }

    native static Zframe __from (String string);
    public Zframe from (String string) {
        return Zframe.__from (string);
    }

    native static Zframe __recv (void * source);
    public Zframe recv (void * source) {
        return Zframe.__recv (source);
    }

    native static int __send (Zframe selfP, void * dest, int flags);
    public int send (Zframe selfP, void * dest, int flags) {
        return Zframe.__send (selfP, dest, flags);
    }

    native static long __size (long pointer);
    public long size (long pointer) {
        return Zframe.__size (pointer);
    }

    native static byte [] __data (long pointer);
    public byte [] data (long pointer) {
        return Zframe.__data (pointer);
    }

    native static Zframe __dup (long pointer);
    public Zframe dup (long pointer) {
        return Zframe.__dup (pointer);
    }

    native static String __strhex (long pointer);
    public String strhex (long pointer) {
        return Zframe.__strhex (pointer);
    }

    native static String __strdup (long pointer);
    public String strdup (long pointer) {
        return Zframe.__strdup (pointer);
    }

    native static boolean __streq (long pointer,String string);
    public boolean streq (long pointer,String string) {
        return Zframe.__streq (pointer,string);
    }

    native static int __more (long pointer);
    public int more (long pointer) {
        return Zframe.__more (pointer);
    }

    native static void __set_more (long pointer,int more);
    public void set_more (long pointer,int more) {
        return Zframe.__set_more (pointer,more);
    }

    native static void __routing_id (long pointer);
    public void routing_id (long pointer) {
        return Zframe.__routing_id (pointer);
    }

    native static void __set_routing_id (long pointer);
    public void set_routing_id (long pointer) {
        return Zframe.__set_routing_id (pointer);
    }

    native static boolean __eq (long pointer,Zframe other);
    public boolean eq (long pointer,Zframe other) {
        return Zframe.__eq (pointer,other);
    }

    native static void __reset (long pointer,void * data, long size);
    public void reset (long pointer,void * data, long size) {
        return Zframe.__reset (pointer,data, size);
    }

    native static void __print (long pointer,String prefix);
    public void print (long pointer,String prefix) {
        return Zframe.__print (pointer,prefix);
    }

    native static boolean __is (void * self);
    public boolean is (void * self) {
        return Zframe.__is (self);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Zframe.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
