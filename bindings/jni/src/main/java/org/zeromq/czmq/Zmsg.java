package org.zeromq.czmq;

public class Zmsg implements AutoCloseable {
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
    public Zmsg (long address) {
        pointer = address;
    }

    native static Zmsg __recv (void * source);
    public Zmsg recv (void * source) {
        return Zmsg.__recv (source);
    }

    native static int __send (Zmsg selfP, void * dest);
    public int send (Zmsg selfP, void * dest) {
        return Zmsg.__send (selfP, dest);
    }

    native static int __sendm (Zmsg selfP, void * dest);
    public int sendm (Zmsg selfP, void * dest) {
        return Zmsg.__sendm (selfP, dest);
    }

    native static long __size (long pointer);
    public long size (long pointer) {
        return Zmsg.__size (pointer);
    }

    native static long __content_size (long pointer);
    public long content_size (long pointer) {
        return Zmsg.__content_size (pointer);
    }

    native static void __routing_id (long pointer);
    public void routing_id (long pointer) {
        return Zmsg.__routing_id (pointer);
    }

    native static void __set_routing_id (long pointer);
    public void set_routing_id (long pointer) {
        return Zmsg.__set_routing_id (pointer);
    }

    native static int __prepend (long pointer,Zframe frameP);
    public int prepend (long pointer,Zframe frameP) {
        return Zmsg.__prepend (pointer,frameP);
    }

    native static int __append (long pointer,Zframe frameP);
    public int append (long pointer,Zframe frameP) {
        return Zmsg.__append (pointer,frameP);
    }

    native static Zframe __pop (long pointer);
    public Zframe pop (long pointer) {
        return Zmsg.__pop (pointer);
    }

    native static int __pushmem (long pointer,void * src, long size);
    public int pushmem (long pointer,void * src, long size) {
        return Zmsg.__pushmem (pointer,src, size);
    }

    native static int __addmem (long pointer,void * src, long size);
    public int addmem (long pointer,void * src, long size) {
        return Zmsg.__addmem (pointer,src, size);
    }

    native static int __pushstr (long pointer,String string);
    public int pushstr (long pointer,String string) {
        return Zmsg.__pushstr (pointer,string);
    }

    native static int __addstr (long pointer,String string);
    public int addstr (long pointer,String string) {
        return Zmsg.__addstr (pointer,string);
    }

    native static int __pushstrf (long pointer, );
    public int pushstrf (long pointer, ) {
        return Zmsg.__pushstrf (pointer,);
    }

    native static int __addstrf (long pointer, );
    public int addstrf (long pointer, ) {
        return Zmsg.__addstrf (pointer,);
    }

    native static String __popstr (long pointer);
    public String popstr (long pointer) {
        return Zmsg.__popstr (pointer);
    }

    native static int __addmsg (long pointer,Zmsg msgP);
    public int addmsg (long pointer,Zmsg msgP) {
        return Zmsg.__addmsg (pointer,msgP);
    }

    native static Zmsg __popmsg (long pointer);
    public Zmsg popmsg (long pointer) {
        return Zmsg.__popmsg (pointer);
    }

    native static void __remove (long pointer,Zframe frame);
    public void remove (long pointer,Zframe frame) {
        return Zmsg.__remove (pointer,frame);
    }

    native static Zframe __first (long pointer);
    public Zframe first (long pointer) {
        return Zmsg.__first (pointer);
    }

    native static Zframe __next (long pointer);
    public Zframe next (long pointer) {
        return Zmsg.__next (pointer);
    }

    native static Zframe __last (long pointer);
    public Zframe last (long pointer) {
        return Zmsg.__last (pointer);
    }

    native static int __save (long pointer);
    public int save (long pointer) {
        return Zmsg.__save (pointer);
    }

    native static Zmsg __load (Zmsg self);
    public Zmsg load (Zmsg self) {
        return Zmsg.__load (self);
    }

    native static long __encode (long pointer,byte [] buffer);
    public long encode (long pointer,byte [] buffer) {
        return Zmsg.__encode (pointer,buffer);
    }

    native static Zmsg __decode (byte [] buffer, long bufferSize);
    public Zmsg decode (byte [] buffer, long bufferSize) {
        return Zmsg.__decode (buffer, bufferSize);
    }

    native static Zmsg __dup (long pointer);
    public Zmsg dup (long pointer) {
        return Zmsg.__dup (pointer);
    }

    native static void __print (long pointer);
    public void print (long pointer) {
        return Zmsg.__print (pointer);
    }

    native static boolean __eq (long pointer,Zmsg other);
    public boolean eq (long pointer,Zmsg other) {
        return Zmsg.__eq (pointer,other);
    }

    native static Zmsg __new_signal (byte status);
    public Zmsg new_signal (byte status) {
        return Zmsg.__new_signal (status);
    }

    native static int __signal (long pointer);
    public int signal (long pointer) {
        return Zmsg.__signal (pointer);
    }

    native static boolean __is (void * self);
    public boolean is (void * self) {
        return Zmsg.__is (self);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Zmsg.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
