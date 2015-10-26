package org.zeromq.czmq;

public class Zlist {
    public native void create ();
    public native void destroy ();
    public native void first ();
    public native void next ();
    public native void last ();
    public native void head ();
    public native void tail ();
    public native void item ();
    public native int append (void Item);
    public native int push (void Item);
    public native void pop ();
    public native boolean exists (void Item);
    public native void remove (void Item);
    public native Zlist dup ();
    public native void purge ();
    public native long size ();
    public native void autofree ();
    public native void test (boolean Verbose);
}
