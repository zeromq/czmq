package org.zeromq.czmq;

public class Zarmour implements AutoCloseable {
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
    public Zarmour (long address) {
        pointer = address;
    }

    native static void __print (long pointer);
    public void print (long pointer) {
        return Zarmour.__print (pointer);
    }

    native static String __mode_str (long pointer);
    public String mode_str (long pointer) {
        return Zarmour.__mode_str (pointer);
    }

    native static String __encode (long pointer,byte [] data, long dataSize);
    public String encode (long pointer,byte [] data, long dataSize) {
        return Zarmour.__encode (pointer,data, dataSize);
    }

    native static byte [] __decode (long pointer,String data, long decodeSize);
    public byte [] decode (long pointer,String data, long decodeSize) {
        return Zarmour.__decode (pointer,data, decodeSize);
    }

    native static void __mode (long pointer);
    public void mode (long pointer) {
        return Zarmour.__mode (pointer);
    }

    native static void __set_mode (long pointer);
    public void set_mode (long pointer) {
        return Zarmour.__set_mode (pointer);
    }

    native static boolean __pad (long pointer);
    public boolean pad (long pointer) {
        return Zarmour.__pad (pointer);
    }

    native static void __set_pad (long pointer,boolean pad);
    public void set_pad (long pointer,boolean pad) {
        return Zarmour.__set_pad (pointer,pad);
    }

    native static void __pad_char (long pointer);
    public void pad_char (long pointer) {
        return Zarmour.__pad_char (pointer);
    }

    native static void __set_pad_char (long pointer);
    public void set_pad_char (long pointer) {
        return Zarmour.__set_pad_char (pointer);
    }

    native static boolean __line_breaks (long pointer);
    public boolean line_breaks (long pointer) {
        return Zarmour.__line_breaks (pointer);
    }

    native static void __set_line_breaks (long pointer,boolean lineBreaks);
    public void set_line_breaks (long pointer,boolean lineBreaks) {
        return Zarmour.__set_line_breaks (pointer,lineBreaks);
    }

    native static long __line_length (long pointer);
    public long line_length (long pointer) {
        return Zarmour.__line_length (pointer);
    }

    native static void __set_line_length (long pointer,long lineLength);
    public void set_line_length (long pointer,long lineLength) {
        return Zarmour.__set_line_length (pointer,lineLength);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Zarmour.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
