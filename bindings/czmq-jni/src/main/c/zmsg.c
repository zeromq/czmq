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
    zmsg_t *msg = (zmsg_t *) self;
    int rc = zmsg_send (&msg, (void *) dest);
    return rc;
}

JNIEXPORT jint JNICALL
Java_org_zeromq_czmq_ZMsg__1_1pushstr (JNIEnv *env, jclass c, jlong ptr, jstring string) {
    const char *str = (const char *) (*env)->GetStringUTFChars (env, string, NULL);
    int rc = zmsg_pushstr ((zmsg_t *) ptr, str);
    (*env)->ReleaseStringUTFChars (env, string, str);
    return rc;
}

JNIEXPORT jstring JNICALL
Java_org_zeromq_czmq_ZMsg__1_1popstr (JNIEnv *env, jclass c, jlong ptr) {
    zmsg_t *msg = (zmsg_t *) ptr;
    char *str = zmsg_popstr (msg);
    // TODO: Unsure if this is a memory leak?
    return (*env)->NewStringUTF (env, str);
}

JNIEXPORT jlong JNICALL
Java_org_zeromq_czmq_ZMsg__1_1pop (JNIEnv *env, jclass c, jlong ptr) {
    zmsg_t *msg = (zmsg_t *) ptr;

    zframe_t *frame = zmsg_pop (msg);
    if (frame) {
        return (jlong) frame;
    }
    return -1;
}

JNIEXPORT jint JNICALL
Java_org_zeromq_czmq_ZMsg__1_1prepend (JNIEnv *env, jclass c, jlong ptr, jlong zframe_ptr) {
    zmsg_t *msg = (zmsg_t *) ptr;
    zframe_t *frame = (zframe_t *) zframe_ptr;
    return zmsg_prepend (msg, &frame);
}

JNIEXPORT jint JNICALL
Java_org_zeromq_czmq_ZMsg__1_1append (JNIEnv *env, jclass c, jlong ptr, jlong zframe_ptr) {
    zmsg_t *msg = (zmsg_t *) ptr;
    zframe_t *frame = (zframe_t *) zframe_ptr;
    return zmsg_append (msg, &frame);
}

JNIEXPORT jlong JNICALL
Java_org_zeromq_czmq_ZMsg__1_1popmsg (JNIEnv *env, jclass c, jlong ptr) {
    zmsg_t *msg = (zmsg_t *) ptr;
    zmsg_t *sub_msg = zmsg_popmsg (msg);
    if (sub_msg) {
        return (jlong) sub_msg;
    }
    return -1;
}

JNIEXPORT jint JNICALL
Java_org_zeromq_czmq_ZMsg__1_1addstr (JNIEnv *env, jclass c, jlong ptr, jstring jstr) {
    zmsg_t *msg = (zmsg_t *) ptr;
    const char *str = (const char *) (*env)->GetStringUTFChars (env, jstr, NULL);
    int rc = zmsg_addstr (msg, str);
    (*env)->ReleaseStringUTFChars (env, jstr, str);
    return rc;
}
