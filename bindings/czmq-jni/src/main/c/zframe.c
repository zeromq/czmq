#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include "czmq.h"
#include "../../native/include/org_zeromq_czmq_ZFrame.h"

JNIEXPORT jlong JNICALL
Java_org_zeromq_czmq_ZFrame__1_1init__ (JNIEnv *env, jclass c) {
    zframe_t *frame = zframe_new_empty();
    if (frame) {
        return (jlong) frame;
    }
    return -1;
}

JNIEXPORT jlong JNICALL
Java_org_zeromq_czmq_ZFrame__1_1init___3BJ (JNIEnv *env, jclass c, jbyteArray buf, jlong size) {
    jbyte *data = (*env)->GetByteArrayElements (env, buf, 0);
    zframe_t *frame = zframe_new(data, (size_t) size);
    (*env)->ReleaseByteArrayElements (env, buf, data, 0);
    if (frame) {
        return (jlong) frame;
    }
    return -1;
}

JNIEXPORT void JNICALL
Java_org_zeromq_czmq_ZFrame__1_1destroy(JNIEnv *env, jclass c, jlong ptr) {
    zframe_t * frame = (zframe_t *) ptr;
    zframe_destroy (&frame);
}

JNIEXPORT jlong JNICALL
Java_org_zeromq_czmq_ZFrame__1_1size (JNIEnv *env, jclass c, jlong ptr) {
    zframe_t *frame = (zframe_t *) ptr;
    size_t size = zframe_size(frame);
    return (jlong) size;
}

JNIEXPORT jbyteArray JNICALL
Java_org_zeromq_czmq_ZFrame__1_1data (JNIEnv *env, jclass c, jlong ptr) {
    zframe_t *frame = (zframe_t *) ptr;
    byte *data = zframe_data (frame);
    size_t size = zframe_size (frame);

    jbyteArray buf = (*env)->NewByteArray (env, (jint) size);
    (*env)->SetByteArrayRegion (env, buf, 0, size, (jbyte*) data);
    return buf;
}
