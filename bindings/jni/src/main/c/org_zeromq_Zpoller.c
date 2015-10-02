#include <jni.h>
#include <stdio.h>
#include "org_zeromq_Zpoller.h"
#include "czmq.h"

JNIEXPORT void JNICALL Java_org_zeromq_Zpoller_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_Zpoller_destroy (JNIEnv *env, jobject thisObj) {
}

//  Add a reader to be polled. Returns 0 if OK, -1 on failure. The reader may
//  be a libzmq void * socket, a zsock_t instance, or a zactor_t instance.   
JNIEXPORT void JNICALL Java_org_zeromq_Zpoller_add (JNIEnv *env, jobject thisObj) {
}

//  Remove a reader from the poller; returns 0 if OK, -1 on failure. The   
//  reader may be a libzmq void * socket, a zsock_t instance, or a zactor_t
//  instance.                                                              
JNIEXPORT void JNICALL Java_org_zeromq_Zpoller_remove (JNIEnv *env, jobject thisObj) {
}

//  Poll the registered readers for I/O, return first reader that has input.  
//  The reader will be a libzmq void * socket, or a zsock_t or zactor_t       
//  instance as specified in zpoller_new/zpoller_add. The timeout should be   
//  zero or greater, or -1 to wait indefinitely. Socket priority is defined   
//  by their order in the poll list. If you need a balanced poll, use the low 
//  level zmq_poll method directly. If the poll call was interrupted (SIGINT),
//  or the ZMQ context was destroyed, or the timeout expired, returns NULL.   
//  You can test the actual exit condition by calling zpoller_expired () and  
//  zpoller_terminated (). The timeout is in msec.                            
JNIEXPORT void JNICALL Java_org_zeromq_Zpoller_wait (JNIEnv *env, jobject thisObj) {
}

//  Return true if the last zpoller_wait () call ended because the timeout
//  expired, without any error.                                           
JNIEXPORT void JNICALL Java_org_zeromq_Zpoller_expired (JNIEnv *env, jobject thisObj) {
}

//  Return true if the last zpoller_wait () call ended because the process
//  was interrupted, or the parent context was destroyed.                 
JNIEXPORT void JNICALL Java_org_zeromq_Zpoller_terminated (JNIEnv *env, jobject thisObj) {
}

//  Ignore zsys_interrupted flag in this poller. By default, a zpoller_wait will 
//  return immediately if detects zsys_interrupted is set to something other than
//  zero. Calling zpoller_ignore_interrupts will supress this behavior.          
JNIEXPORT void JNICALL Java_org_zeromq_Zpoller_ignoreInterrupts (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class
JNIEXPORT void JNICALL Java_org_zeromq_Zpoller_test (JNIEnv *env, jobject thisObj) {
}
