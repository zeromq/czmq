package org.zeromq.czmq;

public class Zsock {
    public native void create ();
    public native void destroy ();
    public native Zsock newPub (String Endpoint);
    public native Zsock newSub (String Endpoint, String Subscribe);
    public native Zsock newReq (String Endpoint);
    public native Zsock newRep (String Endpoint);
    public native Zsock newDealer (String Endpoint);
    public native Zsock newRouter (String Endpoint);
    public native Zsock newPush (String Endpoint);
    public native Zsock newPull (String Endpoint);
    public native Zsock newXpub (String Endpoint);
    public native Zsock newXsub (String Endpoint);
    public native Zsock newPair (String Endpoint);
    public native Zsock newStream (String Endpoint);
    public native Zsock newServer (String Endpoint);
    public native Zsock newClient (String Endpoint);
    public native String endpoint ();
    public native int attach (String Endpoints, boolean Serverish);
    public native String typeStr ();
    public native int send (String Picture,  );
    public native int recv (String Picture,  );
    public native int bsend (String Picture,  );
    public native int brecv (String Picture,  );
    public native void setUnbounded ();
    public native int signal (byte Status);
    public native int wait ();
    public native void flush ();
    public native boolean is (void Self);
    public native void resolve (void Self);
    public native void test (boolean Verbose);
}
