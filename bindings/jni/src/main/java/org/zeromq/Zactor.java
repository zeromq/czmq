package org.zeromq.czmq;

public class Zactor {
    public native void create ();
    public native void destroy ();
    public native int send (Zmsg MsgP);
    public native Zmsg recv ();
    public native boolean is (void Self);
    public native void resolve (void Self);
    public native Zsock sock ();
    public native void test (boolean Verbose);
}
