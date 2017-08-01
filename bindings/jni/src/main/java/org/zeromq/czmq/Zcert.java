/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
*/
package org.zeromq.czmq;

public class Zcert implements AutoCloseable{
    static {
        try {
            System.loadLibrary ("czmqjni");
        }
        catch (Exception e) {
            System.exit (-1);
        }
    }
    public long self;
    /*
    Create and initialize a new certificate in memory
    */
    native static long __new ();
    public Zcert () {
        /*  TODO: if __new fails, self is null...            */
        self = __new ();
    }
    public Zcert (long pointer) {
        self = pointer;
    }
    /*
    Accepts public/secret key pair from caller
    */
    native static long __newFrom (byte [] publicKey, byte [] secretKey);
    public static Zcert newFrom (byte [] publicKey, byte [] secretKey) {
        return new Zcert (__newFrom (publicKey, secretKey));
    }
    /*
    Accepts public/secret key text pair from caller
    */
    native static long __newFromTxt (String publicTxt, String secretTxt);
    public static Zcert newFromTxt (String publicTxt, String secretTxt) {
        return new Zcert (__newFromTxt (publicTxt, secretTxt));
    }
    /*
    Load certificate from file
    */
    native static long __load (String filename);
    public static Zcert load (String filename) {
        return new Zcert (__load (filename));
    }
    /*
    Destroy a certificate in memory
    */
    native static void __destroy (long self);
    @Override
    public void close () {
        __destroy (self);
        self = 0;
    }
    /*
    Return public part of key pair as 32-byte binary string
    */
    native static byte [] __publicKey (long self);
    public byte [] publicKey () {
        return __publicKey (self);
    }
    /*
    Return secret part of key pair as 32-byte binary string
    */
    native static byte [] __secretKey (long self);
    public byte [] secretKey () {
        return __secretKey (self);
    }
    /*
    Return public part of key pair as Z85 armored string
    */
    native static String __publicTxt (long self);
    public String publicTxt () {
        return __publicTxt (self);
    }
    /*
    Return secret part of key pair as Z85 armored string
    */
    native static String __secretTxt (long self);
    public String secretTxt () {
        return __secretTxt (self);
    }
    /*
    Set certificate metadata from formatted string.
    */
    native static void __setMeta (long self, String name, String format);
    public void setMeta (String name, String format) {
        __setMeta (self, name, format);
    }
    /*
    Unset certificate metadata.
    */
    native static void __unsetMeta (long self, String name);
    public void unsetMeta (String name) {
        __unsetMeta (self, name);
    }
    /*
    Get metadata value from certificate; if the metadata value doesn't
    exist, returns NULL.                                              
    */
    native static String __meta (long self, String name);
    public String meta (String name) {
        return __meta (self, name);
    }
    /*
    Get list of metadata fields from certificate. Caller is responsible for
    destroying list. Caller should not modify the values of list items.    
    */
    native static long __metaKeys (long self);
    public Zlist metaKeys () {
        return new Zlist (__metaKeys (self));
    }
    /*
    Save full certificate (public + secret) to file for persistent storage  
    This creates one public file and one secret file (filename + "_secret").
    */
    native static int __save (long self, String filename);
    public int save (String filename) {
        return __save (self, filename);
    }
    /*
    Save public certificate only to file for persistent storage
    */
    native static int __savePublic (long self, String filename);
    public int savePublic (String filename) {
        return __savePublic (self, filename);
    }
    /*
    Save secret certificate only to file for persistent storage
    */
    native static int __saveSecret (long self, String filename);
    public int saveSecret (String filename) {
        return __saveSecret (self, filename);
    }
    /*
    Apply certificate to socket, i.e. use for CURVE security on socket.
    If certificate was loaded from public file, the secret key will be 
    undefined, and this certificate will not work successfully.        
    */
    native static void __apply (long self, long socket);
    public void apply (long socket) {
        __apply (self, socket);
    }
    /*
    Return copy of certificate; if certificate is NULL or we exhausted
    heap memory, returns NULL.                                        
    */
    native static long __dup (long self);
    public Zcert dup () {
        return new Zcert (__dup (self));
    }
    /*
    Return true if two certificates have the same keys
    */
    native static boolean __eq (long self, long compare);
    public boolean eq (Zcert compare) {
        return __eq (self, compare.self);
    }
    /*
    Print certificate contents to stdout
    */
    native static void __print (long self);
    public void print () {
        __print (self);
    }
    /*
    Self test of this class
    */
    native static void __test (boolean verbose);
    public static void test (boolean verbose) {
        __test (verbose);
    }
}
