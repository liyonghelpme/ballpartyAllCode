#include <jni.h>
#include "cocos2d.h"
using namespace cocos2d;
extern "C"{
	JNIEXPORT void JNICALL Java_org_cocos2dx_testcpp_MyAudio_nativeNotification(JNIEnv *env, jobject thiz, jstring msg){
		CCLog("native Notification");

		const char *pt = env->GetStringUTFChars(msg, NULL);

		CCNotificationCenter::sharedNotificationCenter()->postNotification(pt, NULL);
		env->ReleaseStringUTFChars(msg, pt);
	}
}