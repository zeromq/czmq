package org.zeromq.czmq;

public class Zstr {
    public native String recv (void Source);
    public native int recvx (void Source, String StringP,  );
    public native int send (void Dest, String String);
    public native int sendm (void Dest, String String);
    public native int sendx (void Dest, String String,  );
    public native void free (String StringP);
    public native void test (boolean Verbose);
}
