package org.zeromq.czmq;

public class Zsock implements AutoCloseable {
    static {
        try {
            System.loadLibrary ("czmqjni");
        }
        catch (Exception e) {
            System.exit (-1);
        }
    }
    long pointer;

    native static long __init ();
    public () {
        pointer = __init ();
    }
    public Zsock (long address) {
        pointer = address;
    }

    native static Zsock __new_pub (String endpoint);
    public Zsock new_pub (String endpoint) {
        return Zsock.__new_pub (endpoint);
    }

    native static Zsock __new_sub (String endpoint, String subscribe);
    public Zsock new_sub (String endpoint, String subscribe) {
        return Zsock.__new_sub (endpoint, subscribe);
    }

    native static Zsock __new_req (String endpoint);
    public Zsock new_req (String endpoint) {
        return Zsock.__new_req (endpoint);
    }

    native static Zsock __new_rep (String endpoint);
    public Zsock new_rep (String endpoint) {
        return Zsock.__new_rep (endpoint);
    }

    native static Zsock __new_dealer (String endpoint);
    public Zsock new_dealer (String endpoint) {
        return Zsock.__new_dealer (endpoint);
    }

    native static Zsock __new_router (String endpoint);
    public Zsock new_router (String endpoint) {
        return Zsock.__new_router (endpoint);
    }

    native static Zsock __new_push (String endpoint);
    public Zsock new_push (String endpoint) {
        return Zsock.__new_push (endpoint);
    }

    native static Zsock __new_pull (String endpoint);
    public Zsock new_pull (String endpoint) {
        return Zsock.__new_pull (endpoint);
    }

    native static Zsock __new_xpub (String endpoint);
    public Zsock new_xpub (String endpoint) {
        return Zsock.__new_xpub (endpoint);
    }

    native static Zsock __new_xsub (String endpoint);
    public Zsock new_xsub (String endpoint) {
        return Zsock.__new_xsub (endpoint);
    }

    native static Zsock __new_pair (String endpoint);
    public Zsock new_pair (String endpoint) {
        return Zsock.__new_pair (endpoint);
    }

    native static Zsock __new_stream (String endpoint);
    public Zsock new_stream (String endpoint) {
        return Zsock.__new_stream (endpoint);
    }

    native static Zsock __new_server (String endpoint);
    public Zsock new_server (String endpoint) {
        return Zsock.__new_server (endpoint);
    }

    native static Zsock __new_client (String endpoint);
    public Zsock new_client (String endpoint) {
        return Zsock.__new_client (endpoint);
    }

    native static int __bind (long pointer, );
    public int bind (long pointer, ) {
        return Zsock.__bind (pointer,);
    }

    native static String __endpoint (long pointer);
    public String endpoint (long pointer) {
        return Zsock.__endpoint (pointer);
    }

    native static int __unbind (long pointer, );
    public int unbind (long pointer, ) {
        return Zsock.__unbind (pointer,);
    }

    native static int __connect (long pointer, );
    public int connect (long pointer, ) {
        return Zsock.__connect (pointer,);
    }

    native static int __disconnect (long pointer, );
    public int disconnect (long pointer, ) {
        return Zsock.__disconnect (pointer,);
    }

    native static int __attach (long pointer,String endpoints, boolean serverish);
    public int attach (long pointer,String endpoints, boolean serverish) {
        return Zsock.__attach (pointer,endpoints, serverish);
    }

    native static String __type_str (long pointer);
    public String type_str (long pointer) {
        return Zsock.__type_str (pointer);
    }

    native static int __send (long pointer,String picture,  );
    public int send (long pointer,String picture,  ) {
        return Zsock.__send (pointer,picture, );
    }

    native static int __vsend (long pointer,String picture);
    public int vsend (long pointer,String picture) {
        return Zsock.__vsend (pointer,picture);
    }

    native static int __recv (long pointer,String picture,  );
    public int recv (long pointer,String picture,  ) {
        return Zsock.__recv (pointer,picture, );
    }

    native static int __vrecv (long pointer,String picture);
    public int vrecv (long pointer,String picture) {
        return Zsock.__vrecv (pointer,picture);
    }

    native static int __bsend (long pointer,String picture,  );
    public int bsend (long pointer,String picture,  ) {
        return Zsock.__bsend (pointer,picture, );
    }

    native static int __brecv (long pointer,String picture,  );
    public int brecv (long pointer,String picture,  ) {
        return Zsock.__brecv (pointer,picture, );
    }

    native static void __routing_id (long pointer);
    public void routing_id (long pointer) {
        return Zsock.__routing_id (pointer);
    }

    native static void __set_routing_id (long pointer);
    public void set_routing_id (long pointer) {
        return Zsock.__set_routing_id (pointer);
    }

    native static void __set_unbounded (long pointer);
    public void set_unbounded (long pointer) {
        return Zsock.__set_unbounded (pointer);
    }

    native static int __signal (long pointer,byte status);
    public int signal (long pointer,byte status) {
        return Zsock.__signal (pointer,status);
    }

    native static int __wait (long pointer);
    public int wait (long pointer) {
        return Zsock.__wait (pointer);
    }

    native static void __flush (long pointer);
    public void flush (long pointer) {
        return Zsock.__flush (pointer);
    }

    native static boolean __is (void * self);
    public boolean is (void * self) {
        return Zsock.__is (self);
    }

    native static void * __resolve (void * self);
    public void * resolve (void * self) {
        return Zsock.__resolve (self);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Zsock.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
