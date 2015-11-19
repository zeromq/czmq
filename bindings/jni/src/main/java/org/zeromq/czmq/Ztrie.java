package org.zeromq.czmq;

public class Ztrie implements AutoCloseable {
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
    public Ztrie (long address) {
        pointer = address;
    }

    native static void __print (long pointer);
    public void print (long pointer) {
        return Ztrie.__print (pointer);
    }

    native static int __insert_route (long pointer,String path, void * data);
    public int insert_route (long pointer,String path, void * data) {
        return Ztrie.__insert_route (pointer,path, data);
    }

    native static int __remove_route (long pointer,String path);
    public int remove_route (long pointer,String path) {
        return Ztrie.__remove_route (pointer,path);
    }

    native static boolean __matches (long pointer,String path);
    public boolean matches (long pointer,String path) {
        return Ztrie.__matches (pointer,path);
    }

    native static void * __hit_data (long pointer);
    public void * hit_data (long pointer) {
        return Ztrie.__hit_data (pointer);
    }

    native static long __hit_parameter_count (long pointer);
    public long hit_parameter_count (long pointer) {
        return Ztrie.__hit_parameter_count (pointer);
    }

    native static Zhashx __hit_parameters (long pointer);
    public Zhashx hit_parameters (long pointer) {
        return Ztrie.__hit_parameters (pointer);
    }

    native static String __hit_asterisk_match (long pointer);
    public String hit_asterisk_match (long pointer) {
        return Ztrie.__hit_asterisk_match (pointer);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Ztrie.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
