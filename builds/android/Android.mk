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
		zbeacon.c \
		zclock.c \
		zconfig.c \
		zctx.c \
		zfile.c \
		zframe.c \
		zhash.c \
		zlist.c \
		zloop.c \
		zmsg.c \
		zmutex.c \
		zsocket.c \
		zsockopt.c \
		zstr.c \
		zsys.c \
		zthread.c
LOCAL_SHARED_LIBRARIES := zmq
include $(BUILD_SHARED_LIBRARY)
