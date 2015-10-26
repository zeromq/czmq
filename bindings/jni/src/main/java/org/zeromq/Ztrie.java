package org.zeromq.czmq;

public class Ztrie {
    public native void create ();
    public native void destroy ();
    public native void print ();
    public native int removeRoute (String Path);
    public native boolean matches (String Path);
    public native void hitData ();
    public native long hitParameterCount ();
    public native Zhashx hitParameters ();
    public native String hitAsteriskMatch ();
    public native void test (boolean Verbose);
}
