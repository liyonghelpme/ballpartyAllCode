LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := hiredis
LOCAL_CFLAGS := -Wno-error=format-security -std=c99

LOCAL_MODULE_FILENAME := hiredis

LOCAL_C_INCLUDES := $(LOCAL_PATH)/ObjCHiredis/

LOCAL_SRC_FILES := \
ObjCHiredis/hiredis.c \
ObjCHiredis/async.c \
ObjCHiredis/dict.c \
ObjCHiredis/net.c \
ObjCHiredis/sds.c \






LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) 
            
include $(BUILD_STATIC_LIBRARY)

