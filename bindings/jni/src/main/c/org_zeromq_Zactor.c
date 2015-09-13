#include <jni.h>
#include <stdio.h>
#include "org_zeromq_Zactor.h"
#include "czmq.h"

JNIEXPORT void JNICALL Java_org_zeromq_Zactor_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_Zactor_destroy (JNIEnv *env, jobject thisObj) {
}

//  Send a zmsg message to the actor, take ownership of the message
//  and destroy when it has been sent.                             
JNIEXPORT void JNICALL Java_org_zeromq_Zactor_send (JNIEnv *env, jobject thisObj) {
}

//  Receive a zmsg message from the actor. Returns NULL if the actor 
//  was interrupted before the message could be received, or if there
//  was a timeout on the actor.                                      
JNIEXPORT void JNICALL Java_org_zeromq_Zactor_recv (JNIEnv *env, jobject thisObj) {
}

//  Probe the supplied object, and report if it looks like a zactor_t.
JNIEXPORT void JNICALL Java_org_zeromq_Zactor_is (JNIEnv *env, jobject thisObj) {
}

//  Probe the supplied reference. If it looks like a zactor_t instance,
//  return the underlying libzmq actor handle; else if it looks like   
//  a libzmq actor handle, return the supplied value.                  
JNIEXPORT void JNICALL Java_org_zeromq_Zactor_resolve (JNIEnv *env, jobject thisObj) {
}

//  Return the actor's zsock handle. Use this when you absolutely need
//  to work with the zsock instance rather than the actor.            
JNIEXPORT void JNICALL Java_org_zeromq_Zactor_sock (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class.
JNIEXPORT void JNICALL Java_org_zeromq_Zactor_test (JNIEnv *env, jobject thisObj) {
}
