#include "RedisInterface.h"
#include "cocos2d.h"
#include "Logic.h"

#include <jni.h>
#include "JniHelper.h"
#include "SimpleAudioEngine.h"
#include "MyBase64.h"



using namespace CocosDenshion;

#define VOICE "org/cocos2dx/testcpp/MyAudio"
class TestVoice {
private:
	int player;
	int myVid;
	int recorder;

	string recordUrl;

	TestVoice()
	:player(-1)
	,myVid(-1)
	,recorder(-1)
	{

	}
public:
	string  getRecordUrl(){
		return recordUrl;
	}

	int getVid(){
		return myVid;
	}

	static TestVoice *getInstance();

	void playTempVoice(int vid) {
		if (player != -1)
		{
			player = -1;
			CCNotificationCenter::sharedNotificationCenter()->postNotification("finishPlay");
			SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(true);
		}
		if(vid == -1){
			return;
		}

		player = vid;

		char buf[256];
		sprintf(buf, "tempAudio%d.m4a", vid);
		string path = CCFileUtils::sharedFileUtils()->getWritablePath();
		path += buf;

		CCLog("playTempVoice %s", path.c_str());

		//TODO:  如何通知音频播放结束呢? 向上层通知  android delegate 技术或者 不断监测技术
		SimpleAudioEngine::sharedEngine()->playBackgroundMusic(path.c_str(), false);
		CCLog("play voice ok %d", vid);
		//Logic 负责注册 并且 转发 接收到得消息 这里就不注册了
		//PLAY_OVER  
	}
	void startRecord() {
		myVid = Logic::getInstance()->getVid();
		//record 并且存储到特定位置的文件 pcm 或者其它编码 文件
		string path = CCFileUtils::sharedFileUtils()->getWritablePath();
		char buf[256];
		sprintf(buf, "tempAudio%d.m4a", myVid);
		//path += "/";
		path += buf;
		CCLog("store path %s", path.c_str());

		recordUrl = path;

		CCLog("startRecord in c");
		JniMethodInfo t;
		if(JniHelper::getStaticMethodInfo(t, VOICE, "recordAudio", "(Ljava/lang/String;)V")){
			jstring stringArg = t.env->NewStringUTF(path.c_str());
			t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg);
			t.env->DeleteLocalRef(t.classID);
			t.env->DeleteLocalRef(stringArg);
			CCLog("delete local ref finish");
		}

	}
	
	void stopRecord() {
		JniMethodInfo t;
		if(JniHelper::getStaticMethodInfo(t, VOICE, "stopRecord", "()V")){
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
			t.env->DeleteLocalRef(t.classID);
		}
	}

};
std::string storeFile(const unsigned char *con, int len, int vid) {
	char buf[256];
	sprintf(buf, "tempAudio%d.m4a", vid);
	string path = CCFileUtils::sharedFileUtils()->getWritablePath();
	path += buf;

	FILE *f = fopen(path.c_str(), "wb");
	fwrite(con, 1, len, f);
	fclose(f);

	return path;
}


TestVoice *TestVoice::getInstance() {
	static TestVoice *tv = NULL;
	if (tv == NULL)
	{
		tv = new TestVoice();
	}
	return tv;
}

//获取录制音频时间长度
int getVoiceLength(const char *vl) {
	CCLog("get voice file length %s", vl);
	

	JniMethodInfo t;
	if (JniHelper::getStaticMethodInfo(t, VOICE, "getVoiceLength", "(Ljava/lang/String;)I"))
	{
		jstring stringArg = t.env->NewStringUTF(vl);
		jint ret = t.env->CallStaticIntMethod(t.classID, t.methodID, stringArg);
		t.env->DeleteLocalRef(t.classID);
		t.env->DeleteLocalRef(stringArg);

		while(true) {
		    bool res = CCUserDefault::sharedUserDefault()->getBoolForKey("getAudioTime");
		    if (res)
		    {
		    	break;
		    }
		}
		

		int tm = CCUserDefault::sharedUserDefault()->getIntegerForKey("audioTime");
		CCLog("voiceLength is %d", tm);
		//返回剩余 多少秒
		return tm/1000.0f;
	}
	return 0;
}


std::string getFileName() {
	return TestVoice::getInstance()->getRecordUrl();
};

char *encodeVoiceC(const char *fn) {
	CCLog("before send encode voice %s", fn);

	unsigned long fs;
	unsigned char *fdata = CCFileUtils::sharedFileUtils()->getFileData(fn, "r", &fs);
	int olen;
	char *odata = base64_encode((const unsigned char*)fdata, (size_t)fs, (size_t*)&olen);
	free(fdata);

	char *newData = (char*)malloc(olen+1);
	memcpy(newData, odata, olen);
	newData[olen] = '\0';
	free(odata);

	return newData;
}



void *stopRecord() {
	TestVoice::getInstance()->stopRecord();
}


void playVoice(int vid) {
	TestVoice::getInstance()->playTempVoice(vid);
}

void *startRecord() {
	TestVoice::getInstance()->startRecord();
}

int getMyRecordVid() {
	TestVoice *tv = TestVoice::getInstance();
	return tv->getVid();
}
