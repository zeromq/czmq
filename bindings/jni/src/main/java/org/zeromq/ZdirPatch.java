package org.zeromq.czmq;

public class ZdirPatch {
    public native void create ();
    public native void destroy ();
    public native ZdirPatch dup ();
    public native String path ();
    public native Zfile file ();
    public native String vpath ();
    public native void digestSet ();
    public native String digest ();
    public native void test (boolean Verbose);
}
