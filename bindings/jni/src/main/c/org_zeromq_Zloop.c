#include <jni.h>
#include <stdio.h>
#include "org_zeromq_Zloop.h"
#include "czmq.h"

JNIEXPORT void JNICALL Java_org_zeromq_Zloop_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_Zloop_destroy (JNIEnv *env, jobject thisObj) {
}

//  Register socket reader with the reactor. When the reader has messages, 
//  the reactor will call the handler, passing the arg. Returns 0 if OK, -1
//  if there was an error. If you register the same socket more than once, 
//  each instance will invoke its corresponding handler.                   
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_reader (JNIEnv *env, jobject thisObj) {
}

//  Cancel a socket reader from the reactor. If multiple readers exist for
//  same socket, cancels ALL of them.                                     
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_readerEnd (JNIEnv *env, jobject thisObj) {
}

//  Configure a registered reader to ignore errors. If you do not set this,
//  then readers that have errors are removed from the reactor silently.   
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_readerSetTolerant (JNIEnv *env, jobject thisObj) {
}

//  Register low-level libzmq pollitem with the reactor. When the pollitem  
//  is ready, will call the handler, passing the arg. Returns 0 if OK, -1   
//  if there was an error. If you register the pollitem more than once, each
//  instance will invoke its corresponding handler. A pollitem with         
//  socket=NULL and fd=0 means 'poll on FD zero'.                           
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_poller (JNIEnv *env, jobject thisObj) {
}

//  Cancel a pollitem from the reactor, specified by socket or FD. If both
//  are specified, uses only socket. If multiple poll items exist for same
//  socket/FD, cancels ALL of them.                                       
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_pollerEnd (JNIEnv *env, jobject thisObj) {
}

//  Configure a registered poller to ignore errors. If you do not set this,
//  then poller that have errors are removed from the reactor silently.    
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_pollerSetTolerant (JNIEnv *env, jobject thisObj) {
}

//  Register a timer that expires after some delay and repeats some number of
//  times. At each expiry, will call the handler, passing the arg. To run a  
//  timer forever, use 0 times. Returns a timer_id that is used to cancel the
//  timer in the future. Returns -1 if there was an error.                   
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_timer (JNIEnv *env, jobject thisObj) {
}

//  Cancel a specific timer identified by a specific timer_id (as returned by
//  zloop_timer).                                                            
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_timerEnd (JNIEnv *env, jobject thisObj) {
}

//  Register a ticket timer. Ticket timers are very fast in the case where   
//  you use a lot of timers (thousands), and frequently remove and add them. 
//  The main use case is expiry timers for servers that handle many clients, 
//  and which reset the expiry timer for each message received from a client.
//  Whereas normal timers perform poorly as the number of clients grows, the 
//  cost of ticket timers is constant, no matter the number of clients. You  
//  must set the ticket delay using zloop_set_ticket_delay before creating a 
//  ticket. Returns a handle to the timer that you should use in             
//  zloop_ticket_reset and zloop_ticket_delete.                              
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_ticket (JNIEnv *env, jobject thisObj) {
}

//  Reset a ticket timer, which moves it to the end of the ticket list and
//  resets its execution time. This is a very fast operation.             
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_ticketReset (JNIEnv *env, jobject thisObj) {
}

//  Delete a ticket timer. We do not actually delete the ticket here, as    
//  other code may still refer to the ticket. We mark as deleted, and remove
//  later and safely.                                                       
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_ticketDelete (JNIEnv *env, jobject thisObj) {
}

//  Set the ticket delay, which applies to all tickets. If you lower the   
//  delay and there are already tickets created, the results are undefined.
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_setTicketDelay (JNIEnv *env, jobject thisObj) {
}

//  Set hard limit on number of timers allowed. Setting more than a small  
//  number of timers (10-100) can have a dramatic impact on the performance
//  of the reactor. For high-volume cases, use ticket timers. If the hard  
//  limit is reached, the reactor stops creating new timers and logs an    
//  error.                                                                 
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_setMaxTimers (JNIEnv *env, jobject thisObj) {
}

//  Set verbose tracing of reactor on/off
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_setVerbose (JNIEnv *env, jobject thisObj) {
}

//  Start the reactor. Takes control of the thread and returns when the 0MQ  
//  context is terminated or the process is interrupted, or any event handler
//  returns -1. Event handlers may register new sockets and timers, and      
//  cancel sockets. Returns 0 if interrupted, -1 if cancelled by a handler.  
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_start (JNIEnv *env, jobject thisObj) {
}

//  Ignore zsys_interrupted flag in this loop. By default, a zloop_start will 
//  exit as soon as it detects zsys_interrupted is set to something other than
//  zero. Calling zloop_ignore_interrupts will supress this behavior.         
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_ignoreInterrupts (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class
JNIEXPORT void JNICALL Java_org_zeromq_Zloop_test (JNIEnv *env, jobject thisObj) {
}
