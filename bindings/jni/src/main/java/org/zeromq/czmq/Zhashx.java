package org.zeromq.czmq;

public class Zhashx implements AutoCloseable {
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
    public Zhashx (long address) {
        pointer = address;
    }

    native static int __insert (long pointer,void * key, void * item);
    public int insert (long pointer,void * key, void * item) {
        return Zhashx.__insert (pointer,key, item);
    }

    native static void __update (long pointer,void * key, void * item);
    public void update (long pointer,void * key, void * item) {
        return Zhashx.__update (pointer,key, item);
    }

    native static void __delete (long pointer,void * key);
    public void delete (long pointer,void * key) {
        return Zhashx.__delete (pointer,key);
    }

    native static void __purge (long pointer);
    public void purge (long pointer) {
        return Zhashx.__purge (pointer);
    }

    native static void * __lookup (long pointer,void * key);
    public void * lookup (long pointer,void * key) {
        return Zhashx.__lookup (pointer,key);
    }

    native static int __rename (long pointer,void * oldKey, void * newKey);
    public int rename (long pointer,void * oldKey, void * newKey) {
        return Zhashx.__rename (pointer,oldKey, newKey);
    }

    native static void * __freefn (long pointer,void * key);
    public void * freefn (long pointer,void * key) {
        return Zhashx.__freefn (pointer,key);
    }

    native static long __size (long pointer);
    public long size (long pointer) {
        return Zhashx.__size (pointer);
    }

    native static Zlistx __keys (long pointer);
    public Zlistx keys (long pointer) {
        return Zhashx.__keys (pointer);
    }

    native static Zlistx __values (long pointer);
    public Zlistx values (long pointer) {
        return Zhashx.__values (pointer);
    }

    native static void * __first (long pointer);
    public void * first (long pointer) {
        return Zhashx.__first (pointer);
    }

    native static void * __next (long pointer);
    public void * next (long pointer) {
        return Zhashx.__next (pointer);
    }

    native static void * __cursor (long pointer);
    public void * cursor (long pointer) {
        return Zhashx.__cursor (pointer);
    }

    native static void __comment (long pointer,Object... );
    public void comment (long pointer,Object... ) {
        return Zhashx.__comment (pointer,);
    }

    native static int __save (long pointer,String filename);
    public int save (long pointer,String filename) {
        return Zhashx.__save (pointer,filename);
    }

    native static int __load (long pointer,String filename);
    public int load (long pointer,String filename) {
        return Zhashx.__load (pointer,filename);
    }

    native static int __refresh (long pointer);
    public int refresh (long pointer) {
        return Zhashx.__refresh (pointer);
    }

    native static Zframe __pack (long pointer);
    public Zframe pack (long pointer) {
        return Zhashx.__pack (pointer);
    }

    native static Zhashx __unpack (Zframe frame);
    public Zhashx unpack (Zframe frame) {
        return Zhashx.__unpack (frame);
    }

    native static Zhashx __dup (long pointer);
    public Zhashx dup (long pointer) {
        return Zhashx.__dup (pointer);
    }

    native static void __set_destructor (long pointer);
    public void set_destructor (long pointer) {
        return Zhashx.__set_destructor (pointer);
    }

    native static void __set_duplicator (long pointer);
    public void set_duplicator (long pointer) {
        return Zhashx.__set_duplicator (pointer);
    }

    native static void __set_key_destructor (long pointer);
    public void set_key_destructor (long pointer) {
        return Zhashx.__set_key_destructor (pointer);
    }

    native static void __set_key_duplicator (long pointer);
    public void set_key_duplicator (long pointer) {
        return Zhashx.__set_key_duplicator (pointer);
    }

    native static void __set_key_comparator (long pointer);
    public void set_key_comparator (long pointer) {
        return Zhashx.__set_key_comparator (pointer);
    }

    native static void __set_key_hasher (long pointer);
    public void set_key_hasher (long pointer) {
        return Zhashx.__set_key_hasher (pointer);
    }

    native static Zhashx __dup_v2 (long pointer);
    public Zhashx dup_v2 (long pointer) {
        return Zhashx.__dup_v2 (pointer);
    }

    native static void __autofree (long pointer);
    public void autofree (long pointer) {
        return Zhashx.__autofree (pointer);
    }

    native static int __foreach (long pointer,void * argument);
    public int foreach (long pointer,void * argument) {
        return Zhashx.__foreach (pointer,argument);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Zhashx.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
