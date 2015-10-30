#include <jni.h>
#include <stdio.h>
#include "org_zeromq_Zframe.h"
#include "czmq.h"

JNIEXPORT void JNICALL Java_org_zeromq_Zframe_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_Zframe_destroy (JNIEnv *env, jobject thisObj) {
}

//  Create an empty (zero-sized) frame
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_newEmpty (JNIEnv *env, jobject thisObj) {
}

//  Create a frame with a specified string content.
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_from (JNIEnv *env, jobject thisObj) {
}

//  Receive frame from socket, returns zframe_t object or NULL if the recv  
//  was interrupted. Does a blocking recv, if you want to not block then use
//  zpoller or zloop.                                                       
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_recv (JNIEnv *env, jobject thisObj) {
}

//  Send a frame to a socket, destroy frame after sending.
//  Return -1 on error, 0 on success.                     
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_send (JNIEnv *env, jobject thisObj) {
}

//  Return number of bytes in frame data
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_size (JNIEnv *env, jobject thisObj) {
}

//  Return address of frame data
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_data (JNIEnv *env, jobject thisObj) {
}

//  Create a new frame that duplicates an existing frame. If frame is null,
//  or memory was exhausted, returns null.                                 
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_dup (JNIEnv *env, jobject thisObj) {
}

//  Return frame data encoded as printable hex string, useful for 0MQ UUIDs.
//  Caller must free string when finished with it.                          
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_strhex (JNIEnv *env, jobject thisObj) {
}

//  Return frame data copied into freshly allocated string
//  Caller must free string when finished with it.        
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_strdup (JNIEnv *env, jobject thisObj) {
}

//  Return TRUE if frame body is equal to string, excluding terminator
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_streq (JNIEnv *env, jobject thisObj) {
}

//  Return frame MORE indicator (1 or 0), set when reading frame from socket
//  or by the zframe_set_more() method                                      
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_more (JNIEnv *env, jobject thisObj) {
}

//  Set frame MORE indicator (1 or 0). Note this is NOT used when sending
//  frame to socket, you have to specify flag explicitly.                
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_setMore (JNIEnv *env, jobject thisObj) {
}

//  Return frame routing ID, if the frame came from a ZMQ_SERVER socket.
//  Else returns zero.                                                  
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_routingId (JNIEnv *env, jobject thisObj) {
}

//  Set routing ID on frame. This is used if/when the frame is sent to a
//  ZMQ_SERVER socket.                                                  
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_setRoutingId (JNIEnv *env, jobject thisObj) {
}

//  Return TRUE if two frames have identical size and data
//  If either frame is NULL, equality is always false.    
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_eq (JNIEnv *env, jobject thisObj) {
}

//  Set new contents for frame
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_reset (JNIEnv *env, jobject thisObj) {
}

//  Send message to zsys log sink (may be stdout, or system facility as       
//  configured by zsys_set_logstream). Prefix shows before frame, if not null.
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_print (JNIEnv *env, jobject thisObj) {
}

//  Probe the supplied object, and report if it looks like a zframe_t.
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_is (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class
JNIEXPORT void JNICALL Java_org_zeromq_Zframe_test (JNIEnv *env, jobject thisObj) {
}
