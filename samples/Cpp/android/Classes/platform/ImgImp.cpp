#include "RedisInterface.h"
#include <jni.h>
#include "JniHelper.h"
#include "cocos2d.h"

using namespace cocos2d;

#define IMAGE "org/cocos2dx/testcpp/MyImage"
/*
extern "C" {
	JNIEXPORT void JNICALL Java_org_cocos2dx_testcpp_MyImage_nativeSaveImageData(JNIEnv *env, jobject thiz, jbyteArray text);
	JNIEXPORT void JNICALL Java_org_cocos2dx_testcpp_MyImage_nativeStartGetImage(JNIEnv *env);
}

*/



class ImgSelect {
private:
	bool inGetImageData, getYet;
	char *imgdata;
	int len;
public:
	bool getGetYet() {
		return getYet;
	}
	char *getData() {
		return imgdata;
	}
	int getLen() {
		return len;
	}

	void setImgData(char *p, int l) {
		if (imgdata != NULL)
		{
			free(imgdata);
			imgdata = NULL;
		}
		imgdata = p;
		len = l;

		inGetImageData = false;
		getYet = true;
		CCLog("get ImageData success");
	}

	
	void startGetImage(){
		inGetImageData = true;
	}

	ImgSelect()
	:inGetImageData(false)
	,getYet(false)
	,imgdata(NULL)
	,len(0)
	{

	}
	static ImgSelect *getInstance();
	void showImgSelect() {
		CCLog("show Image Select");

		if(inGetImageData)
			return;
		getYet = false;
		JniMethodInfo t;
		if(JniHelper::getStaticMethodInfo(t, IMAGE, "showImgSelect", "()V")){
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
			t.env->DeleteLocalRef(t.classID);
			CCLog("showImageSelect");
		}
	}
};

ImgSelect *ImgSelect::getInstance() {
	static ImgSelect *is = NULL;
	if(is == NULL){
		is = new ImgSelect();
	}
	return is;
}

extern "C" {
	//多线程处理 传入的 图像数据 选择的数据 经历两个 操作 缩放 压缩  拷贝
	JNIEXPORT void JNICALL Java_org_cocos2dx_testcpp_MyImage_nativeSaveImageData(JNIEnv *env, jobject thiz, jbyteArray text){
		jsize size = env->GetArrayLength(text);
		if(size > 0) {
			jbyte *data = (jbyte*)env->GetByteArrayElements(text, 0);
			

			char *pBuf = (char*)malloc(size);
			if(pBuf != NULL) {
				memcpy(pBuf, data, size);
			}
			ImgSelect::getInstance()->setImgData(pBuf, size);


			env->ReleaseByteArrayElements(text, data, 0);
		}else {

		}

	}
	//
	JNIEXPORT void JNICALL Java_org_cocos2dx_testcpp_MyImage_nativeStartGetImage(JNIEnv *env){
		ImgSelect::getInstance()->startGetImage();
	}
}




void openImageSelect() {
	ImgSelect::getInstance()->showImgSelect();
}

bool checkGetYet() {
	return ImgSelect::getInstance()->getGetYet();
}

void *getImage(int *length) {
	CCLog("getImage Data");
	if(ImgSelect::getInstance()->getData() != NULL) {
		*length = ImgSelect::getInstance()->getLen();
		return ImgSelect::getInstance()->getData();
	}else {
		*length = 0;
	}
	return NULL;
}



