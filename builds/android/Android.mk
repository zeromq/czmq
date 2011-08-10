BASE_PATH := $(call my-dir)
APP_PLATFORM = android-10

LOCAL_PATH := $(BASE_PATH)

#Info for libzmq
include $(CLEAR_VARS)
LOCAL_MODULE := zmq
LOCAL_SRC_FILES := libzmq.so
include $(PREBUILT_SHARED_LIBRARY)


# build czmq
include $(CLEAR_VARS)
LOCAL_MODULE := czmq
LOCAL_C_INCLUDES := ../../include $(LIBZMQ)/include
LOCAL_SRC_FILES := \
		zclock.c zctx.c zlist.c zsockopt.c zsocket.c zmsg.c \
		zloop.c zthread.c zstr.c zframe.c zhash.c
LOCAL_SHARED_LIBRARIES := zmq
include $(BUILD_SHARED_LIBRARY)
