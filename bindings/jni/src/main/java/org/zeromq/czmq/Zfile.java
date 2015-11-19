package org.zeromq.czmq;

public class Zfile implements AutoCloseable {
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
    public Zfile (long address) {
        pointer = address;
    }

    native static Zfile __dup (long pointer);
    public Zfile dup (long pointer) {
        return Zfile.__dup (pointer);
    }

    native static String __filename (long pointer,String path);
    public String filename (long pointer,String path) {
        return Zfile.__filename (pointer,path);
    }

    native static void __restat (long pointer);
    public void restat (long pointer) {
        return Zfile.__restat (pointer);
    }

    native static long __modified (long pointer);
    public long modified (long pointer) {
        return Zfile.__modified (pointer);
    }

    native static long __cursize (long pointer);
    public long cursize (long pointer) {
        return Zfile.__cursize (pointer);
    }

    native static boolean __is_directory (long pointer);
    public boolean is_directory (long pointer) {
        return Zfile.__is_directory (pointer);
    }

    native static boolean __is_regular (long pointer);
    public boolean is_regular (long pointer) {
        return Zfile.__is_regular (pointer);
    }

    native static boolean __is_readable (long pointer);
    public boolean is_readable (long pointer) {
        return Zfile.__is_readable (pointer);
    }

    native static boolean __is_writeable (long pointer);
    public boolean is_writeable (long pointer) {
        return Zfile.__is_writeable (pointer);
    }

    native static boolean __is_stable (long pointer);
    public boolean is_stable (long pointer) {
        return Zfile.__is_stable (pointer);
    }

    native static boolean __has_changed (long pointer);
    public boolean has_changed (long pointer) {
        return Zfile.__has_changed (pointer);
    }

    native static void __remove (long pointer);
    public void remove (long pointer) {
        return Zfile.__remove (pointer);
    }

    native static int __input (long pointer);
    public int input (long pointer) {
        return Zfile.__input (pointer);
    }

    native static int __output (long pointer);
    public int output (long pointer) {
        return Zfile.__output (pointer);
    }

    native static Zchunk __read (long pointer,long bytes, long offset);
    public Zchunk read (long pointer,long bytes, long offset) {
        return Zfile.__read (pointer,bytes, offset);
    }

    native static boolean __eof (long pointer);
    public boolean eof (long pointer) {
        return Zfile.__eof (pointer);
    }

    native static int __write (long pointer,Zchunk chunk, long offset);
    public int write (long pointer,Zchunk chunk, long offset) {
        return Zfile.__write (pointer,chunk, offset);
    }

    native static String __readln (long pointer);
    public String readln (long pointer) {
        return Zfile.__readln (pointer);
    }

    native static void __close (long pointer);
    public void close (long pointer) {
        return Zfile.__close (pointer);
    }

    native static void __handle (long pointer);
    public void handle (long pointer) {
        return Zfile.__handle (pointer);
    }

    native static String __digest (long pointer);
    public String digest (long pointer) {
        return Zfile.__digest (pointer);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Zfile.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
