package org.zeromq.czmq;

public class Zfile {
    public native void create ();
    public native void destroy ();
    public native Zfile dup ();
    public native String filename (String Path);
    public native void restat ();
    public native long modified ();
    public native long cursize ();
    public native boolean isDirectory ();
    public native boolean isRegular ();
    public native boolean isReadable ();
    public native boolean isWriteable ();
    public native boolean isStable ();
    public native boolean hasChanged ();
    public native void remove ();
    public native int input ();
    public native int output ();
    public native Zchunk read (long Bytes, long Offset);
    public native boolean eof ();
    public native int write (Zchunk Chunk, long Offset);
    public native String readln ();
    public native void close ();
    public native String digest ();
    public native void test (boolean Verbose);
}
