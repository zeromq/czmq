package org.zeromq.czmq;

public class Zhash implements AutoCloseable {
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
    public Zhash (long address) {
        pointer = address;
    }

    native static int __insert (long pointer,String key, void * item);
    public int insert (long pointer,String key, void * item) {
        return Zhash.__insert (pointer,key, item);
    }

    native static void __update (long pointer,String key, void * item);
    public void update (long pointer,String key, void * item) {
        return Zhash.__update (pointer,key, item);
    }

    native static void __delete (long pointer,String key);
    public void delete (long pointer,String key) {
        return Zhash.__delete (pointer,key);
    }

    native static void * __lookup (long pointer,String key);
    public void * lookup (long pointer,String key) {
        return Zhash.__lookup (pointer,key);
    }

    native static int __rename (long pointer,String oldKey, String newKey);
    public int rename (long pointer,String oldKey, String newKey) {
        return Zhash.__rename (pointer,oldKey, newKey);
    }

    native static void * __freefn (long pointer,String key);
    public void * freefn (long pointer,String key) {
        return Zhash.__freefn (pointer,key);
    }

    native static long __size (long pointer);
    public long size (long pointer) {
        return Zhash.__size (pointer);
    }

    native static Zhash __dup (long pointer);
    public Zhash dup (long pointer) {
        return Zhash.__dup (pointer);
    }

    native static Zlist __keys (long pointer);
    public Zlist keys (long pointer) {
        return Zhash.__keys (pointer);
    }

    native static void * __first (long pointer);
    public void * first (long pointer) {
        return Zhash.__first (pointer);
    }

    native static void * __next (long pointer);
    public void * next (long pointer) {
        return Zhash.__next (pointer);
    }

    native static String __cursor (long pointer);
    public String cursor (long pointer) {
        return Zhash.__cursor (pointer);
    }

    native static void __comment (long pointer,Object... );
    public void comment (long pointer,Object... ) {
        return Zhash.__comment (pointer,);
    }

    native static Zframe __pack (long pointer);
    public Zframe pack (long pointer) {
        return Zhash.__pack (pointer);
    }

    native static Zhash __unpack (Zframe frame);
    public Zhash unpack (Zframe frame) {
        return Zhash.__unpack (frame);
    }

    native static int __save (long pointer,String filename);
    public int save (long pointer,String filename) {
        return Zhash.__save (pointer,filename);
    }

    native static int __load (long pointer,String filename);
    public int load (long pointer,String filename) {
        return Zhash.__load (pointer,filename);
    }

    native static int __refresh (long pointer);
    public int refresh (long pointer) {
        return Zhash.__refresh (pointer);
    }

    native static void __autofree (long pointer);
    public void autofree (long pointer) {
        return Zhash.__autofree (pointer);
    }

    native static int __foreach (long pointer,void * argument);
    public int foreach (long pointer,void * argument) {
        return Zhash.__foreach (pointer,argument);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Zhash.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
