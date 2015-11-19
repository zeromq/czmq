package org.zeromq.czmq;

public class Zdir implements AutoCloseable {
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
    public Zdir (long address) {
        pointer = address;
    }

    native static String __path (long pointer);
    public String path (long pointer) {
        return Zdir.__path (pointer);
    }

    native static long __modified (long pointer);
    public long modified (long pointer) {
        return Zdir.__modified (pointer);
    }

    native static long __cursize (long pointer);
    public long cursize (long pointer) {
        return Zdir.__cursize (pointer);
    }

    native static long __count (long pointer);
    public long count (long pointer) {
        return Zdir.__count (pointer);
    }

    native static Zlist __list (long pointer);
    public Zlist list (long pointer) {
        return Zdir.__list (pointer);
    }

    native static void __remove (long pointer,boolean force);
    public void remove (long pointer,boolean force) {
        return Zdir.__remove (pointer,force);
    }

    native static Zlist __diff (Zdir older, Zdir newer, String alias);
    public Zlist diff (Zdir older, Zdir newer, String alias) {
        return Zdir.__diff (older, newer, alias);
    }

    native static Zlist __resync (long pointer,String alias);
    public Zlist resync (long pointer,String alias) {
        return Zdir.__resync (pointer,alias);
    }

    native static Zhash __cache (long pointer);
    public Zhash cache (long pointer) {
        return Zdir.__cache (pointer);
    }

    native static void __fprint (long pointer,int indent);
    public void fprint (long pointer,int indent) {
        return Zdir.__fprint (pointer,indent);
    }

    native static void __print (long pointer,int indent);
    public void print (long pointer,int indent) {
        return Zdir.__print (pointer,indent);
    }

    native static void __watch (Zsock pipe, void * unused);
    public void watch (Zsock pipe, void * unused) {
        return Zdir.__watch (pipe, unused);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Zdir.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
