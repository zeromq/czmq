package org.zeromq.czmq;

public class Zpoller {
    public native void create ();
    public native void destroy ();
    public native int add (void Reader);
    public native int remove (void Reader);
    public native void wait (int Timeout);
    public native boolean expired ();
    public native boolean terminated ();
    public native void ignoreInterrupts ();
    public native void test (boolean Verbose);
}
