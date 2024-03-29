/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
*/
package org.zeromq.czmq;

import org.zeromq.tools.ZmqNativeLoader;

import java.util.LinkedHashMap;
import java.util.Map;

public class Zclock {
    static {
        Map<String, Boolean> libraries = new LinkedHashMap<>();
        libraries.put("zmq", false);
        libraries.put("uuid", true);
        libraries.put("systemd", true);
        libraries.put("lz4", true);
        libraries.put("curl", true);
        libraries.put("nss", true);
        libraries.put("microhttpd", true);
        libraries.put("czmq", false);
        libraries.put("czmqjni", false);
        ZmqNativeLoader.loadLibraries(libraries);
    }
    public long self;
    /*
    Sleep for a number of milliseconds
    */
    native static void __sleep (int msecs);
    public static void sleep (int msecs) {
        __sleep (msecs);
    }
    /*
    Return current system clock as milliseconds. Note that this clock can
    jump backwards (if the system clock is changed) so is unsafe to use for
    timers and time offsets. Use zclock_mono for that instead.
    */
    native static long __time ();
    public static long time () {
        return __time ();
    }
    /*
    Return current monotonic clock in milliseconds. Use this when you compute
    time offsets. The monotonic clock is not affected by system changes and
    so will never be reset backwards, unlike a system clock.
    */
    native static long __mono ();
    public static long mono () {
        return __mono ();
    }
    /*
    Return current monotonic clock in microseconds. Use this when you compute
    time offsets. The monotonic clock is not affected by system changes and
    so will never be reset backwards, unlike a system clock.
    */
    native static long __usecs ();
    public static long usecs () {
        return __usecs ();
    }
    /*
    Return formatted date/time as fresh string. Free using zstr_free().
    */
    native static String __timestr ();
    public static String timestr () {
        return __timestr ();
    }
    /*
    Self test of this class.
    */
    native static void __test (boolean verbose);
    public static void test (boolean verbose) {
        __test (verbose);
    }
}
