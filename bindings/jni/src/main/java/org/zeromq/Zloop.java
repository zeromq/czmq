package org.zeromq.czmq;

public class Zloop {
    public native void create ();
    public native void destroy ();
    public native void readerEnd (Zsock Sock);
    public native void readerSetTolerant (Zsock Sock);
    public native int timerEnd (int TimerId);
    public native void ticketReset (void Handle);
    public native void ticketDelete (void Handle);
    public native void setTicketDelay (long TicketDelay);
    public native void setMaxTimers (long MaxTimers);
    public native void setVerbose (boolean Verbose);
    public native int start ();
    public native void ignoreInterrupts ();
    public native void test (boolean Verbose);
}
