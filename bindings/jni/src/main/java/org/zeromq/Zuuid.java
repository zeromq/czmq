package org.zeromq.czmq;

public class Zuuid {
    public native void create ();
    public native void destroy ();
    public native void print ();
    public native Zuuid newFrom (byte [] Source);
    public native void set (byte [] Source);
    public native int setStr (String Source);
    public native byte [] data ();
    public native long size ();
    public native String str ();
    public native String strCanonical ();
    public native void export (byte [] Target);
    public native boolean eq (byte [] Compare);
    public native boolean neq (byte [] Compare);
    public native Zuuid dup ();
    public native void test (boolean Verbose);
}
