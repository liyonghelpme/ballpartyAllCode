//
//  ChatView.h
//  TestCpp
//
//  Created by dozengame on 14-5-15.
//
//

#ifndef __TestCpp__ChatView__
#define __TestCpp__ChatView__

#include <iostream>
#include "cocos-ext.h"
#include "cocos2d.h"

#include "Channel.h"


using namespace cocos2d;
using namespace extension;
using namespace ui;
class Message;
//class Channel;

class ChatView : public CCLayer, public CCIMEDelegate {
public:
    virtual bool init();
    virtual void update(float);
    static cocos2d::CCScene* scene(int cid);
    CREATE_FUNC(ChatView);
    
    //获取键盘相关信息
    virtual void keyboardWillShow(CCIMEKeyboardNotificationInfo& info)   {
        //CC_UNUSED_PARAM(info);
        CCLog("begin info %f %f %f %f", info.begin.origin.x, info.begin.origin.y, info.begin.size.width, info.begin.size.height);
        CCLog("end info %f %f %f %f", info.end.origin.x, info.end.origin.y, info.end.size.width, info.end.size.height);
        CCLog("dura info %f", info.duration);
        ksize = info.end.size;
        setSizeYet = true;
        
        pinyinMove();
    }
    
    virtual void keyboardDidShow(CCIMEKeyboardNotificationInfo& info)    {CC_UNUSED_PARAM(info);}
    virtual void keyboardWillHide(CCIMEKeyboardNotificationInfo& info)   {CC_UNUSED_PARAM(info);}
    virtual void keyboardDidHide(CCIMEKeyboardNotificationInfo& info)    {CC_UNUSED_PARAM(info);}
    
    ~ChatView();
    virtual void onEnter();
    virtual void onExit();


    virtual void keyBackClicked();
    
private:
    void checkKeyBoard();



    CCPoint oldPos;
    
    CCArray *errorMsg;
    void checkError();
    
    CCLabelTTF *chatText;
    
    void closeRedis();
    
    bool closeYet;
    
    void closeChat();
    
    bool firstGetMsg;
    
    bool initReceiveYet;
    
    Label *overTime;
    
    void stopRecordAndSend();
    void onResumeGame(CCObject*);
    
    float recordTime;
    bool isRecording;
    
    void updateRecordTime(float);
    
    
    bool insertHistory;
    
    void onPauseGame(CCObject *);
    
    void onScroll(CCObject *obj, ScrollviewEventType st);
    void adjustSize();
    void adjustAbs();
    ImageView *bottomBack;
    CCSize bbackSize;
    
    
    float delaySizeTime;
    
    int waitScrollFrame;
    
    float marginHeight;
    
    void startAdjustSize(float eh);
    float sizeTime;
    bool needAdjust;
    
    
    float totalTime;
    float startHeight;
    float endHeight;
    float totalWidth;
    
    
    //调整midView的尺寸
    void updateMidSize(float diff);
    
    
    
    bool inUpdate;
    float sleepTime;
    
    ImageView *inputBorder;
    ImageView *ani;
    CCDictionary *voiceMap;
    
    void onFinishPlay(CCObject*);
    ImageView *playingVoice;
    CCSpriteFrame *dframe;
    
    int scrollTime;
    void setNeedScroll() {
        needScroll = 0;
        scrollTime = 0;
    }
    int needScroll;
    void onImgBnt(CCObject *but, TouchEventType);
    bool initYet;
    void reconnectGetHistory(long long);
    void insertMessage(Message *msg, int ord);
    int m_matchID;
    Channel *m_channel;
    
    std::string getVoiceTime(int);
    
    ImageView *inImage, *cancelImg, *bigImg;
    
    Widget *w;
    Label *timeNow;
    float passTime;
    
    float getLabelWidth();
    float getVoiceWidth(float vl, Widget*);
    
    void onText(CCObject *, TextFiledEventType);
    void onSend(CCObject*, TouchEventType);
    void onSpeak(CCObject*, TouchEventType);
    void onSay(CCObject*   , TouchEventType);
    void onOtherVoice(CCObject *, TouchEventType);
    void onChatInfo(CCObject*, TouchEventType);
    void onAdd(CCObject*, TouchEventType);
    void onEnterChat(bool isSuc, std::string s, void *);
    
    void onInRecord(CCObject*, TouchEventType);
    
    //出现拼音输入法的时候键盘还得再移动一下
    void pinyinMove();
    
    Layout *myImg, *otherImg;

    void updateEnter(float);
    float lastUpdateTime;
    
    
    //listView 的两个事件不是同时可用的 有项目的时候这个可用
    void onBackground(CCObject*, ListViewEventType);
    //没有项目的时候 该事件可用
    void onBackgroundTouch(CCObject*, TouchEventType);
    
    void onVoice(CCObject*, TouchEventType);
    void onMsg(bool isSuc, std::string s, void *param);
    void sendImg();
    
    void adjustBut();
    
    void receiveMsg();
    
    void trySendMsg(string &con, enum Channel::MESSAGE_TYPE mt, ImageView *error);
    
    UIPanel *oneWord;
    //UIButton *content;
    UIPanel *twoWord;
    UILabel *lab2;
    ImageView *head2;
    
    
    UIPanel *bottom;
    CCSize ksize;
    bool setSizeYet;
    TextField *tf;
    ListView *lv;
    
    UILabel *lab;
    ImageView *head;
    
    CCLabelTTF *testLabel;
    UIButton *speak;
    //ImageView *voice2;
    
    Button *sayWord;
    
    bool inVoice;
    
    Layout *myvoice;
    //ImageView *myvImg;
    Button *myvImg;
    
    //int vid;
    
    Layout *otherVoice;
    UIImageView *ohead;
    Button *ovoice;
    Button *backbnt;
    
    Layout *inRecord;
    Label *curTime;
    
    Layout *midView;

    void onBack(CCObject*, TouchEventType);
    
    bool showKeyboard;
    
    
    UIButton *add, *send;
    
    bool selectImgYet;

    
    bool enterChatYet;
    
    int state;
    
    
    CCPoint oldSizePer;
    //int chatImgId;
    
    //float getInputWidth();
};
#endif /* defined(__TestCpp__ChatView__) */
