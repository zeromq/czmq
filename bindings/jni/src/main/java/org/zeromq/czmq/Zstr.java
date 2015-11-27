package org.zeromq.czmq;

public class Zstr implements AutoCloseable {
    static {
        try {
            System.loadLibrary ("czmqjni");
        }
        catch (Exception e) {
            System.exit (-1);
        }
    }
    long pointer;

    native static String __recv (void * source);
    public String recv (void * source) {
        return Zstr.__recv (source);
    }

    native static int __recvx (void * source, String stringP, Object... );
    public int recvx (void * source, String stringP, Object... ) {
        return Zstr.__recvx (source, stringP, );
    }

    native static int __send (void * dest, String string);
    public int send (void * dest, String string) {
        return Zstr.__send (dest, string);
    }

    native static int __sendm (void * dest, String string);
    public int sendm (void * dest, String string) {
        return Zstr.__sendm (dest, string);
    }

    native static int __sendf (void * dest, Object... );
    public int sendf (void * dest, Object... ) {
        return Zstr.__sendf (dest, );
    }

    native static int __sendfm (void * dest, Object... );
    public int sendfm (void * dest, Object... ) {
        return Zstr.__sendfm (dest, );
    }

    native static int __sendx (void * dest, String string, Object... );
    public int sendx (void * dest, String string, Object... ) {
        return Zstr.__sendx (dest, string, );
    }

    native static void __free (String stringP);
    public void free (String stringP) {
        return Zstr.__free (stringP);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Zstr.__test (verbose);
    }

}
