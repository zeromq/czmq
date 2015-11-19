package org.zeromq.czmq;

public class Zloop implements AutoCloseable {
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
    public Zloop (long address) {
        pointer = address;
    }

    native static int __reader (long pointer,Zsock sock, void * arg);
    public int reader (long pointer,Zsock sock, void * arg) {
        return Zloop.__reader (pointer,sock, arg);
    }

    native static void __reader_end (long pointer,Zsock sock);
    public void reader_end (long pointer,Zsock sock) {
        return Zloop.__reader_end (pointer,sock);
    }

    native static void __reader_set_tolerant (long pointer,Zsock sock);
    public void reader_set_tolerant (long pointer,Zsock sock) {
        return Zloop.__reader_set_tolerant (pointer,sock);
    }

    native static int __poller (long pointer,void * arg);
    public int poller (long pointer,void * arg) {
        return Zloop.__poller (pointer,arg);
    }

    native static void __poller_end (long pointer);
    public void poller_end (long pointer) {
        return Zloop.__poller_end (pointer);
    }

    native static void __poller_set_tolerant (long pointer);
    public void poller_set_tolerant (long pointer) {
        return Zloop.__poller_set_tolerant (pointer);
    }

    native static int __timer (long pointer,long delay, long times, void * arg);
    public int timer (long pointer,long delay, long times, void * arg) {
        return Zloop.__timer (pointer,delay, times, arg);
    }

    native static int __timer_end (long pointer,int timerId);
    public int timer_end (long pointer,int timerId) {
        return Zloop.__timer_end (pointer,timerId);
    }

    native static void * __ticket (long pointer,void * arg);
    public void * ticket (long pointer,void * arg) {
        return Zloop.__ticket (pointer,arg);
    }

    native static void __ticket_reset (long pointer,void * handle);
    public void ticket_reset (long pointer,void * handle) {
        return Zloop.__ticket_reset (pointer,handle);
    }

    native static void __ticket_delete (long pointer,void * handle);
    public void ticket_delete (long pointer,void * handle) {
        return Zloop.__ticket_delete (pointer,handle);
    }

    native static void __set_ticket_delay (long pointer,long ticketDelay);
    public void set_ticket_delay (long pointer,long ticketDelay) {
        return Zloop.__set_ticket_delay (pointer,ticketDelay);
    }

    native static void __set_max_timers (long pointer,long maxTimers);
    public void set_max_timers (long pointer,long maxTimers) {
        return Zloop.__set_max_timers (pointer,maxTimers);
    }

    native static void __set_verbose (long pointer,boolean verbose);
    public void set_verbose (long pointer,boolean verbose) {
        return Zloop.__set_verbose (pointer,verbose);
    }

    native static int __start (long pointer);
    public int start (long pointer) {
        return Zloop.__start (pointer);
    }

    native static void __ignore_interrupts (long pointer);
    public void ignore_interrupts (long pointer) {
        return Zloop.__ignore_interrupts (pointer);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Zloop.__test (verbose);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
