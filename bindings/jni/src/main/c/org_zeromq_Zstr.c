#include <jni.h>
#include <stdio.h>
#include "org_zeromq_Zstr.h"
#include "czmq.h"

//  Receive C string from socket. Caller must free returned string using
//  zstr_free(). Returns NULL if the context is being terminated or the 
//  process was interrupted.                                            
JNIEXPORT void JNICALL Java_org_zeromq_Zstr_recv (JNIEnv *env, jobject thisObj) {
}

//  Receive a series of strings (until NULL) from multipart data.    
//  Each string is allocated and filled with string data; if there   
//  are not enough frames, unallocated strings are set to NULL.      
//  Returns -1 if the message could not be read, else returns the    
//  number of strings filled, zero or more. Free each returned string
//  using zstr_free(). If not enough strings are provided, remaining 
//  multipart frames in the message are dropped.                     
JNIEXPORT void JNICALL Java_org_zeromq_Zstr_recvx (JNIEnv *env, jobject thisObj) {
}

//  Send a C string to a socket, as a frame. The string is sent without 
//  trailing null byte; to read this you can use zstr_recv, or a similar
//  method that adds a null terminator on the received string. String   
//  may be NULL, which is sent as "".                                   
JNIEXPORT void JNICALL Java_org_zeromq_Zstr_send (JNIEnv *env, jobject thisObj) {
}

//  Send a C string to a socket, as zstr_send(), with a MORE flag, so that
//  you can send further strings in the same multi-part message.          
JNIEXPORT void JNICALL Java_org_zeromq_Zstr_sendm (JNIEnv *env, jobject thisObj) {
}

//  Send a formatted string to a socket. Note that you should NOT use
//  user-supplied strings in the format (they may contain '%' which  
//  will create security holes).                                     
JNIEXPORT void JNICALL Java_org_zeromq_Zstr_sendf (JNIEnv *env, jobject thisObj) {
}

//  Send a formatted string to a socket, as for zstr_sendf(), with a      
//  MORE flag, so that you can send further strings in the same multi-part
//  message.                                                              
JNIEXPORT void JNICALL Java_org_zeromq_Zstr_sendfm (JNIEnv *env, jobject thisObj) {
}

//  Send a series of strings (until NULL) as multipart data   
//  Returns 0 if the strings could be sent OK, or -1 on error.
JNIEXPORT void JNICALL Java_org_zeromq_Zstr_sendx (JNIEnv *env, jobject thisObj) {
}

//  Free a provided string, and nullify the parent pointer. Safe to call on
//  a null pointer.                                                        
JNIEXPORT void JNICALL Java_org_zeromq_Zstr_free (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class
JNIEXPORT void JNICALL Java_org_zeromq_Zstr_test (JNIEnv *env, jobject thisObj) {
}
