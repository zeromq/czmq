package org.zeromq.czmq;

public class Zhash {
    public native void create ();
    public native void destroy ();
    public native int insert (String Key, void Item);
    public native void update (String Key, void Item);
    public native void delete (String Key);
    public native void lookup (String Key);
    public native int rename (String OldKey, String NewKey);
    public native long size ();
    public native Zhash dup ();
    public native Zlist keys ();
    public native void first ();
    public native void next ();
    public native String cursor ();
    public native Zframe pack ();
    public native Zhash unpack (Zframe Frame);
    public native int save (String Filename);
    public native int load (String Filename);
    public native int refresh ();
    public native void autofree ();
    public native void test (boolean Verbose);
}
