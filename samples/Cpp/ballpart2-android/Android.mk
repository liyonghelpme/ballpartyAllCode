LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos_testcpp_common
LOCAL_CFLAGS := -Wno-error=format-security

LOCAL_MODULE_FILENAME := libtestcppcommon

LOCAL_C_INCLUDES := $(LOCAL_PATH)/Classes/Scene $(LOCAL_PATH)/Classes $(LOCAL_PATH)/Classes/platform $(LOCAL_PATH)/../../../cocos2dx/support $(LOCAL_PATH)/../../../extensions/CocoStudio/Json/rapidjson $(LOCAL_PATH)/Classes/Common/coverView
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../ObjCHiredis-master/ObjCHiredis
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../cocos2dx/platform/android/jni
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../CocosDenshion/include

LOCAL_SRC_FILES := \
Classes/platform/voiceImp.cpp \
Classes/AppDelegate.cpp \
Classes/Scene/LoginScene.cpp \
Classes/ServiceBase.cpp \
Classes/ServiceCenter.cpp \
Classes/Channel.cpp \
Classes/HttpModel.cpp \
Classes/ChatInfo.cpp \
Classes/ChatView.cpp \
Classes/Common/coverView/CoverView.cpp \
Classes/ConfigView.cpp \
Classes/Logic.cpp \
Classes/Match.cpp \
Classes/MatchService.cpp \
Classes/Md5.cpp \
Classes/Message.cpp \
Classes/MessageService.cpp \
Classes/MyBase64.cpp \
Classes/Scene/RegistScene.cpp \
Classes/User.cpp \
Classes/UserService.cpp \
Classes/WorldCup.cpp \
Classes/platform/RedisImp.cpp \
Classes/platform/Java_org_cocos2dx_testcpp_MyAudio.cpp \
Classes/platform/ImgImp.cpp \




LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
LOCAL_WHOLE_STATIC_LIBRARIES += chipmunk_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static
LOCAL_WHOLE_STATIC_LIBRARIES += hiredis

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) 
            
include $(BUILD_STATIC_LIBRARY)

$(call import-module,CocosDenshion/android)
$(call import-module,external/Box2D)
$(call import-module,external/chipmunk)
$(call import-module,cocos2dx)
$(call import-module,extensions)
$(call import-module,samples/Cpp/ballpart2-android/hiredis)
