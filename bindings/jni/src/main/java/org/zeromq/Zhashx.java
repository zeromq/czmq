package org.zeromq.czmq;

public class Zhashx {
    public native void create ();
    public native void destroy ();
    public native int insert (void Key, void Item);
    public native void update (void Key, void Item);
    public native void delete (void Key);
    public native void purge ();
    public native void lookup (void Key);
    public native int rename (void OldKey, void NewKey);
    public native long size ();
    public native Zlistx keys ();
    public native Zlistx values ();
    public native void first ();
    public native void next ();
    public native void cursor ();
    public native int save (String Filename);
    public native int load (String Filename);
    public native int refresh ();
    public native Zframe pack ();
    public native Zhashx unpack (Zframe Frame);
    public native Zhashx dup ();
    public native Zhashx dupV2 ();
    public native void autofree ();
    public native void test (boolean Verbose);
}
