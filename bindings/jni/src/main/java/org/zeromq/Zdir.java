package org.zeromq.czmq;

public class Zdir {
    public native void create ();
    public native void destroy ();
    public native String path ();
    public native long modified ();
    public native long cursize ();
    public native long count ();
    public native Zlist list ();
    public native void remove (boolean Force);
    public native Zlist diff (Zdir Older, Zdir Newer, String Alias);
    public native Zlist resync (String Alias);
    public native Zhash cache ();
    public native void print (int Indent);
    public native void watch (Zsock Pipe, void Unused);
    public native void test (boolean Verbose);
}
