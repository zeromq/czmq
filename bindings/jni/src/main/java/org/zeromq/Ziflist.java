package org.zeromq.czmq;

public class Ziflist {
    public native void create ();
    public native void destroy ();
    public native void print ();
    public native void reload ();
    public native long size ();
    public native String first ();
    public native String next ();
    public native String address ();
    public native String broadcast ();
    public native String netmask ();
    public native void test (boolean Verbose);
}
