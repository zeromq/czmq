package org.zeromq.czmq;

public class Zmsg {
    public native void create ();
    public native void destroy ();
    public native Zmsg recv (void Source);
    public native int send (Zmsg SelfP, void Dest);
    public native int sendm (Zmsg SelfP, void Dest);
    public native long size ();
    public native long contentSize ();
    public native int prepend (Zframe FrameP);
    public native int append (Zframe FrameP);
    public native Zframe pop ();
    public native int pushmem (void Src, long Size);
    public native int addmem (void Src, long Size);
    public native int pushstr (String String);
    public native int addstr (String String);
    public native String popstr ();
    public native int addmsg (Zmsg MsgP);
    public native Zmsg popmsg ();
    public native void remove (Zframe Frame);
    public native Zframe first ();
    public native Zframe next ();
    public native Zframe last ();
    public native long encode (byte [] Buffer);
    public native Zmsg decode (byte [] Buffer, long BufferSize);
    public native Zmsg dup ();
    public native void print ();
    public native boolean eq (Zmsg Other);
    public native Zmsg newSignal (byte Status);
    public native int signal ();
    public native boolean is (void Self);
    public native void test (boolean Verbose);
}
