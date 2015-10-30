package org.zeromq.czmq;

public class Zframe {
    public native void create ();
    public native void destroy ();
    public native Zframe newEmpty ();
    public native Zframe from (String String);
    public native Zframe recv (void Source);
    public native int send (Zframe SelfP, void Dest, int Flags);
    public native long size ();
    public native byte [] data ();
    public native Zframe dup ();
    public native String strhex ();
    public native String strdup ();
    public native boolean streq (String String);
    public native int more ();
    public native void setMore (int More);
    public native boolean eq (Zframe Other);
    public native void reset (void Data, long Size);
    public native void print (String Prefix);
    public native boolean is (void Self);
    public native void test (boolean Verbose);
}
