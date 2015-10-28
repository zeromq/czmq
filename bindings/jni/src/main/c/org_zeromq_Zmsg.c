#include <jni.h>
#include <stdio.h>
#include "org_zeromq_Zmsg.h"
#include "czmq.h"

JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_destroy (JNIEnv *env, jobject thisObj) {
}

//  Receive message from socket, returns zmsg_t object or NULL if the recv   
//  was interrupted. Does a blocking recv. If you want to not block then use 
//  the zloop class or zmsg_recv_nowait or zmq_poll to check for socket input
//  before receiving.                                                        
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_recv (JNIEnv *env, jobject thisObj) {
}

//  Send message to destination socket, and destroy the message after sending
//  it successfully. If the message has no frames, sends nothing but destroys
//  the message anyhow. Nullifies the caller's reference to the message (as  
//  it is a destructor).                                                     
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_send (JNIEnv *env, jobject thisObj) {
}

//  Send message to destination socket as part of a multipart sequence, and 
//  destroy the message after sending it successfully. Note that after a    
//  zmsg_sendm, you must call zmsg_send or another method that sends a final
//  message part. If the message has no frames, sends nothing but destroys  
//  the message anyhow. Nullifies the caller's reference to the message (as 
//  it is a destructor).                                                    
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_sendm (JNIEnv *env, jobject thisObj) {
}

//  Return size of message, i.e. number of frames (0 or more).
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_size (JNIEnv *env, jobject thisObj) {
}

//  Return total size of all frames in message.
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_contentSize (JNIEnv *env, jobject thisObj) {
}

//  Return message routing ID, if the message came from a ZMQ_SERVER socket.
//  Else returns zero.                                                      
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_routingId (JNIEnv *env, jobject thisObj) {
}

//  Set routing ID on message. This is used if/when the message is sent to a
//  ZMQ_SERVER socket.                                                      
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_setRoutingId (JNIEnv *env, jobject thisObj) {
}

//  Push frame to the front of the message, i.e. before all other frames.  
//  Message takes ownership of frame, will destroy it when message is sent.
//  Returns 0 on success, -1 on error. Deprecates zmsg_push, which did not 
//  nullify the caller's frame reference.                                  
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_prepend (JNIEnv *env, jobject thisObj) {
}

//  Add frame to the end of the message, i.e. after all other frames.      
//  Message takes ownership of frame, will destroy it when message is sent.
//  Returns 0 on success. Deprecates zmsg_add, which did not nullify the   
//  caller's frame reference.                                              
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_append (JNIEnv *env, jobject thisObj) {
}

//  Remove first frame from message, if any. Returns frame, or NULL.
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_pop (JNIEnv *env, jobject thisObj) {
}

//  Push block of memory to front of message, as a new frame.
//  Returns 0 on success, -1 on error.                       
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_pushmem (JNIEnv *env, jobject thisObj) {
}

//  Add block of memory to the end of the message, as a new frame.
//  Returns 0 on success, -1 on error.                            
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_addmem (JNIEnv *env, jobject thisObj) {
}

//  Push string as new frame to front of message.
//  Returns 0 on success, -1 on error.           
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_pushstr (JNIEnv *env, jobject thisObj) {
}

//  Push string as new frame to end of message.
//  Returns 0 on success, -1 on error.         
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_addstr (JNIEnv *env, jobject thisObj) {
}

//  Push formatted string as new frame to front of message.
//  Returns 0 on success, -1 on error.                     
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_pushstrf (JNIEnv *env, jobject thisObj) {
}

//  Push formatted string as new frame to end of message.
//  Returns 0 on success, -1 on error.                   
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_addstrf (JNIEnv *env, jobject thisObj) {
}

//  Pop frame off front of message, return as fresh string. If there were
//  no more frames in the message, returns NULL.                         
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_popstr (JNIEnv *env, jobject thisObj) {
}

//  Push encoded message as a new frame. Message takes ownership of    
//  submessage, so the original is destroyed in this call. Returns 0 on
//  success, -1 on error.                                              
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_addmsg (JNIEnv *env, jobject thisObj) {
}

//  Remove first submessage from message, if any. Returns zmsg_t, or NULL if
//  decoding was not succesfull.                                            
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_popmsg (JNIEnv *env, jobject thisObj) {
}

//  Remove specified frame from list, if present. Does not destroy frame.
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_remove (JNIEnv *env, jobject thisObj) {
}

//  Set cursor to first frame in message. Returns frame, or NULL, if the
//  message is empty. Use this to navigate the frames as a list.        
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_first (JNIEnv *env, jobject thisObj) {
}

//  Return the next frame. If there are no more frames, returns NULL. To move
//  to the first frame call zmsg_first(). Advances the cursor.               
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_next (JNIEnv *env, jobject thisObj) {
}

//  Return the last frame. If there are no frames, returns NULL.
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_last (JNIEnv *env, jobject thisObj) {
}

//  Save message to an open file, return 0 if OK, else -1. The message is  
//  saved as a series of frames, each with length and data. Note that the  
//  file is NOT guaranteed to be portable between operating systems, not   
//  versions of CZMQ. The file format is at present undocumented and liable
//  to arbitrary change.                                                   
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_save (JNIEnv *env, jobject thisObj) {
}

//  Load/append an open file into message, create new message if
//  null message provided. Returns NULL if the message could not
//  be loaded.                                                  
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_load (JNIEnv *env, jobject thisObj) {
}

//  Serialize multipart message to a single buffer. Use this method to send  
//  structured messages across transports that do not support multipart data.
//  Allocates and returns a new buffer containing the serialized message.    
//  To decode a serialized message buffer, use zmsg_decode ().               
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_encode (JNIEnv *env, jobject thisObj) {
}

//  Decodes a serialized message buffer created by zmsg_encode () and returns
//  a new zmsg_t object. Returns NULL if the buffer was badly formatted or   
//  there was insufficient memory to work.                                   
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_decode (JNIEnv *env, jobject thisObj) {
}

//  Create copy of message, as new message object. Returns a fresh zmsg_t
//  object. If message is null, or memory was exhausted, returns null.   
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_dup (JNIEnv *env, jobject thisObj) {
}

//  Send message to zsys log sink (may be stdout, or system facility as
//  configured by zsys_set_logstream).                                 
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_print (JNIEnv *env, jobject thisObj) {
}

//  Return true if the two messages have the same number of frames and each  
//  frame in the first message is identical to the corresponding frame in the
//  other message. As with zframe_eq, return false if either message is NULL.
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_eq (JNIEnv *env, jobject thisObj) {
}

//  Generate a signal message encoding the given status. A signal is a short
//  message carrying a 1-byte success/failure code (by convention, 0 means  
//  OK). Signals are encoded to be distinguishable from "normal" messages.  
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_newSignal (JNIEnv *env, jobject thisObj) {
}

//  Return signal value, 0 or greater, if message is a signal, -1 if not.
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_signal (JNIEnv *env, jobject thisObj) {
}

//  Probe the supplied object, and report if it looks like a zmsg_t.
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_is (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class
JNIEXPORT void JNICALL Java_org_zeromq_Zmsg_test (JNIEnv *env, jobject thisObj) {
}
