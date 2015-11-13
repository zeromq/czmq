#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include "czmq.h"
#include "../../native/include/org_zeromq_czmq_ZMsg.h"

JNIEXPORT jlong JNICALL
Java_org_zeromq_czmq_ZMsg__1_1init__ (JNIEnv *env, jclass c) {
    zmsg_t *msg = zmsg_new ();
    if (msg) {
        return (jlong) msg;
    }
    return -1;
}

JNIEXPORT void JNICALL
Java_org_zeromq_czmq_ZMsg__1_1destroy (JNIEnv *env, jclass c, jlong ptr) {
    zmsg_t *msg = (zmsg_t *) ptr;
    zmsg_destroy (&msg);
}

JNIEXPORT jint JNICALL
Java_org_zeromq_czmq_ZMsg__1_1size (JNIEnv *env, jclass c, jlong ptr) {
    size_t size = zmsg_size ((zmsg_t *) ptr);
    return (jlong) size;
}

JNIEXPORT jlong JNICALL
Java_org_zeromq_czmq_ZMsg__1_1content_1size (JNIEnv *env, jclass c, jlong ptr) {
    size_t size = zmsg_content_size ((zmsg_t *) ptr);
    return (jlong) size;
}

JNIEXPORT jlong JNICALL
Java_org_zeromq_czmq_ZMsg__1_1recv (JNIEnv *env, jclass c, jlong source) {
    zmsg_t *msg = zmsg_recv ((void *) source);
    return (jlong) msg;
}

JNIEXPORT jlong JNICALL
Java_org_zeromq_czmq_ZMsg__1_1send (JNIEnv *env, jclass c, jlong self, jlong dest) {
    int rc = zmsg_send((zmsg_t **) self, (void *) dest);
    return rc;
}

JNIEXPORT jint JNICALL
Java_org_zeromq_czmq_ZMsg__1_1pushstr (JNIEnv *env, jclass c, jlong ptr, jstring string) {
    const char *str = (const char *) (*env)->GetStringUTFChars (env, string, NULL);
    int rc = zmsg_pushstr ((zmsg_t *) ptr, str);
    (*env)->ReleaseStringUTFChars (env, string, str);
    return rc;
}