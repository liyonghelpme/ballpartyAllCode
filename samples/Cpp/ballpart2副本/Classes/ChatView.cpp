//
//  ChatView.cpp
//  TestCpp
//
//  Created by dozengame on 14-5-15.
//
//

#include "ChatView.h"
#include "RedisInterface.h"
#include "stringbuffer.h"
#include "writer.h"
#include "base64.h"
#include "string.h"
#include "Logic.h"
#include "ChatInfo.h"
#include "MyBase64.h"
#include "Channel.h"
#include "Message.h"
#include "MessageService.h"
#include "ServiceCenter.h"


float textMargin = 35;
float imgWidthMargin = 40;
float imgHeightMargin = 25;

float FONT_SIZE = 30;
float INPUT_SIZE = 30;

float HEAD_SIZE = 70;

float TEXT_WIDTH = 395;


CCScene *ChatView::scene(int mID){
    CCScene *scene = CCScene::create();
    
    
    // 'layer' is an autorelease object
    ChatView *layer = ChatView::create();
    layer->m_matchID = mID;
    //MatchService *ms = (MatchService*)ServiceCenter::getInstance()->getService(ServiceCenter::MATCH_SERVICE);
    //Match *mat = ms->getMatchById(mid);
    
    MessageService *messageService = (MessageService*)ServiceCenter::getInstance()->getService(ServiceCenter::MESSAGE_SERVICE);
    layer->m_channel = messageService->openChannel(mID);
    CCLog("open Channel Here %d", mID);
    
    
    
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}
//TODO: Channel delete 
ChatView::~ChatView(){
    errorMsg->release();
    testLabel->release();
    chatText->release();
    
    //voiceMap->release();
}
void ChatView::onFinishPlay(cocos2d::CCObject *obj){
    CCLog("receive finish play message");
    if (playingVoice != NULL) {
        playingVoice->getRender()->stopAllActions();
        playingVoice->getRender()->setDisplayFrame(dframe);
        playingVoice = NULL;
        dframe->release();
        dframe = NULL;
    }
}


void ChatView::closeRedis() {
    if (closeYet) {
        return;
    }
    Logic::getInstance()->inChatRoom = false;
    
    //等再次进入重新连接接收redis
    startReceiveRedis(-1);
    initReceiveYet = false;
    CCLog("退出场景则关闭redis 连接 closeRedis");
    
    
    //发出一条离开聊天室的消息
    string emp;
    bool suc = m_channel->sendMessage(emp, Channel::QUIT_TYPE, -1);
    //关闭发送redis 连接
    //closeSendRedisC();
    startSendRedis(-1);
    
}


void ChatView::onExit(){
    curInScene = NULL;
    CCLog("ChatView onExit");
    
    playVoice(-1);
    
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "finishPlay");
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "pauseGame");
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "resumeGame");
    tf->closeIME();
    closeRedis();
    CCLayer::onExit();
}


//停止所有语音播放
void ChatView::onPauseGame(cocos2d::CCObject *obj) {
    CCLog("ChatView pause game");
    playVoice(-1);
    
    //直接退出房间即可
    closeChat();
    
    
    //退出游戏的时候 清理一下 接收线程对象 然后 再进入的时候 恢复一下即可 lostConnection的时间自己记录一下
    //线程清内存存在问题
    //startReceiveRedis(-1);
    //发出一条离开聊天室的消息
    //string emp;
    //bool suc = m_channel->sendMessage(emp, Channel::QUIT_TYPE);
}


//resumeGame 之后 重新连接 redis的socket socket已经失效了 无法写入
void ChatView::onResumeGame(CCObject* obj) {
    CCLog("resume game");
    reconnectRedis();
}

void ChatView::onEnter() {
    CCLayer::onEnter();
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(ChatView::onFinishPlay), "finishPlay", NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(ChatView::onPauseGame), "pauseGame", NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(ChatView::onResumeGame), "resumeGame", NULL);
    
    
    if (initYet) {
        return;
    }
    initYet = true;

    
    CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
    MatchService *ms = (MatchService*)ServiceCenter::getInstance()->getService(ServiceCenter::MATCH_SERVICE);
    Match *minf = ms->getMatchById(m_matchID);
    
    Label *title = static_cast<Label*>(UIHelper::seekWidgetByName(w, "title"));
    //if (!DEBUG) {
    
    string tname = minf->host_name;
    tname += "VS";
    tname += minf->guest_name;
    if (m_matchID == FEEDBACK) {
        //反馈
        title->setText("球友圈");
    }else
        title->setText(tname);
    
    curTime = static_cast<Label*>(UIHelper::seekWidgetByName(w, "time"));
    
    overTime = static_cast<Label*>(UIHelper::seekWidgetByName(w, "overTime"));
    overTime->setEnabled(false);
    
    
    //输入框文本太大了
    tf = static_cast<UITextField*>(UIHelper::seekWidgetByName(w, "word"));
    tf->setMulLine(true);
    tf->setMaxLength(125);
    tf->setFontSize(INPUT_SIZE);
    oldPos = tf->getPosition();
    
    
    
    //根据系统判定 字符数量  根据系统高度判定
    //CCSize *fs = CCDirector::sharedDirector()->getVisibleSize();
    //itouch4 设置 字体大小小一些即可
    oneWord = static_cast<UIPanel*>(UIHelper::seekWidgetByName(w, "oneWord"));
    lab = static_cast<UILabel*>(UIHelper::seekWidgetByName(oneWord, "userDialog"));
    
    
    twoWord = static_cast<UIPanel*>(UIHelper::seekWidgetByName(w, "twoWord"));
    lab2 = static_cast<UILabel*>(UIHelper::seekWidgetByName(twoWord, "userDialog2"));
    
    //都限制70个字符
    //if(fabs(fs.height-960.0) < 1) {
        tf->setMaxLength(70);
        testLabel->setFontSize(FONT_SIZE);
        lab->setFontSize(FONT_SIZE);
        lab2->setFontSize(FONT_SIZE);
        CCLog("itouch 4 set font size");
    //}
    
    
    bottom = static_cast<UIPanel*>(UIHelper::seekWidgetByName(w, "bottom"));
    Layout *wordLayer = static_cast<Layout*>(UIHelper::seekWidgetByName(bottom, "wordLayer"));
    //float wh = wordLayer->getSize().height;
    //wordLayer->setSize(CCSizeMake(fs.width, wh));
    
    
    bottomBack = static_cast<ImageView*>(UIHelper::seekWidgetByName(bottom, "bottomBack"));
    bbackSize = bottomBack->getSize();
    
    
    
    inputBorder = static_cast<ImageView*>(UIHelper::seekWidgetByName(w, "inputBorder"));
    
    
    CCSize bs = tf->getSize();
    tf->ignoreContentAdaptWithSize(false);
    float nw = 400*fs.width/640;
    
    
    
    //输入框高度随着输入内容一起增长即可
    tf->setSize(CCSizeMake(nw, 58));
    
    tf->setTextHorizontalAlignment(kCCTextAlignmentLeft);
    tf->setTextVerticalAlignment(kCCVerticalTextAlignmentBottom);
    tf->setAnchorPoint(ccp(0, 0));
    tf->setSizeType(SIZE_ABSOLUTE);
    
    tf->addEventListenerTextField(this, textfieldeventselector(ChatView::onText));
    
    
    add = static_cast<UIButton*>(UIHelper::seekWidgetByName(bottom, "add"));
    //add->setEnabled(false);
    add->addTouchEventListener(this, toucheventselector(ChatView::onAdd));
    
    
    send = static_cast<UIButton*>(UIHelper::seekWidgetByName(bottom, "send"));
    send->addTouchEventListener(this, toucheventselector(ChatView::onSend));
    send->setEnabled(false);
    send->setVisible(true);
    
    
    
    //对方说话的panel
    //panel 下面还可以增加panel么?
    
    oneWord->setEnabled(false);
    
    lab->ignoreContentAdaptWithSize(false);
    head = static_cast<ImageView*>(UIHelper::seekWidgetByName(oneWord, "head"));
    
    
    //float lwid = fs.width-114-10-20;
    
    //CCLog("label max widht %f", lwid);
    //lab->setSizeType(SIZE_ABSOLUTE);
    //lab->setTextAreaSize(CCSizeMake(100, fs.height));
    
    
    
    twoWord->setEnabled(false);
    
    
    lab2->ignoreContentAdaptWithSize(false);
    head2 = static_cast<ImageView*>(UIHelper::seekWidgetByName(twoWord, "head2"));
    //voice2 = static_cast<ImageView*>(UIHelper::seekWidgetByName(twoWord, "voice2"));
    
    speak = static_cast<Button*>(UIHelper::seekWidgetByName(bottom, "speak"));
    speak->setEnabled(false);
    speak->addTouchEventListener(this, toucheventselector(ChatView::onSpeak));
    
    backbnt = static_cast<Button*>(UIHelper::seekWidgetByName(w, "back"));
    backbnt->addTouchEventListener(this, toucheventselector(ChatView::onBack));
    
    
    
    sayWord = static_cast<Button*>(UIHelper::seekWidgetByName(bottom, "voice"));
    sayWord->addTouchEventListener(this, toucheventselector(ChatView::onSay));
    myvoice = static_cast<UIPanel*>(UIHelper::seekWidgetByName(w, "myvoice"));
    myvImg = static_cast<Button*>(UIHelper::seekWidgetByName(myvoice, "voice2"));
    myvoice->setEnabled(false);
    
    
    myImg = static_cast<Layout*>(UIHelper::seekWidgetByName(w, "myImg"));
    myImg->setEnabled(false);
    otherImg = static_cast<Layout*>(UIHelper::seekWidgetByName(w, "otherImg"));
    otherImg->setEnabled(false);
    
    
    
    
    otherVoice = static_cast<Layout*>(UIHelper::seekWidgetByName(w, "otherVoice"));
    ohead = static_cast<ImageView*>(UIHelper::seekWidgetByName(otherVoice, "ohead"));
    ovoice = static_cast<Button*>(UIHelper::seekWidgetByName(otherVoice, "ovoice"));
    otherVoice->setEnabled(false);
    ovoice->addTouchEventListener(this, toucheventselector(ChatView::onOtherVoice));
    
    
    ImageView *myImgHead = static_cast<ImageView*>(UIHelper::seekWidgetByName(myImg, "head2"));
    ImageView *otherImgHead = static_cast<ImageView*>(UIHelper::seekWidgetByName(otherImg, "head"));
    ImageView *mhead = static_cast<ImageView*>(UIHelper::seekWidgetByName(myvoice, "head3"));
    
    CCSize hsz = CCSizeMake(HEAD_SIZE, HEAD_SIZE);
    //oneWord
    head->setSize(hsz);
    head2->setSize(hsz);
    ohead->setSize(hsz);
    mhead->setSize(hsz);
    myImgHead->setSize(hsz);
    otherImgHead->setSize(hsz);
    
    
    
    //反馈页面不能看用户信息
    Button *cinfo = static_cast<Button*>(UIHelper::seekWidgetByName(w, "selfConfig"));
    cinfo->addTouchEventListener(this, toucheventselector(ChatView::onChatInfo));
    if (m_channel->cid == FEEDBACK) {
        cinfo->setEnabled(false);
    }
    
    
    
    CCLog("initial listView");
    
    lv = static_cast<ListView*>(UIHelper::seekWidgetByName(w, "listView"));
    lv->addTouchEventListener(this, toucheventselector(ChatView::onBackgroundTouch));
    lv->addEventListenerListView(this, listvieweventselector(ChatView::onBackground));
    
    //拖到最上面获取当前的历史消息
    lv->addEventListenerScrollView(this, scrollvieweventselector(ChatView::onScroll));
    
    //1/4 缩小了 bounce的范围
    lv->setBoundParam(5.0f);
}
void ChatView::onScroll(cocos2d::CCObject *obj, ScrollviewEventType st) {
    CCLog("scroll event %d", st);
    switch (st) {
            //取消该功能
            
            //放手后回到顶部开始
        case cocos2d::ui::SCROLLVIEW_EVENT_SCROLL_TO_TOP:
        {
            //获取历史消息 已经初始化过消息了 向上拖动获取历史消息
            //消息总数 少于 20条 才获取更多的历史消息
            if (state == 2 && sleepTime < 0 && lv->getItems()->count() < 20) {
                state = 1;
                inUpdate = true;
                sleepTime = 10;
                MessageService *ms = (MessageService*)ServiceCenter::getInstance()->getService(ServiceCenter::MESSAGE_SERVICE);
                ms->getHistoryMessage(m_channel->cid, this, MYHTTP_SEL(ChatView::onMsg), false);
            }
            
        }
            break;
            //放手后 回到底部开始
        case cocos2d::ui::SCROLLVIEW_EVENT_SCROLL_TO_BOTTOM:
        {
            
        }
            break;
            
        default:
            break;
    }
}

bool ChatView::init(){
    if ( !CCLayer::init() )
    {
        return false;
    }
    Logic::getInstance()->inChatRoom = true;
    
    errorMsg = CCArray::create();
    errorMsg->retain();
    
    
    closeYet = false;
    
    initReceiveYet = false;
    
    //第一次获取历史消息
    firstGetMsg = false;
    
    recordTime = 0;
    isRecording = false;
    
    marginHeight = 30;
    
    waitScrollFrame = 5;
    
    totalTime = 0.23;
    sizeTime = 0;
    needAdjust = false;
    
    delaySizeTime = 0.0f;
    
    sleepTime = 0;
    inUpdate = false;
    initYet = false;
    needScroll = 0;
    
    //chatImgId = 1;
    showKeyboard = false;
    inVoice = false;
    //vid = 0;
    selectImgYet = false;
    enterChatYet = false;
    lastUpdateTime = 0;
    state = 0;
    
    //voiceMap = CCDictionary::create();
    //voiceMap->retain();
    
    playingVoice = NULL;
    
    
    setSizeYet = false;
    //设置显示的文本的大小和高度
    testLabel = CCLabelTTF::create("", "", INPUT_SIZE);
    testLabel->retain();
    
    chatText = CCLabelTTF::create("", "", INPUT_SIZE);
    chatText->retain();
    
    
    CCSize size = CCDirector::sharedDirector()->getVisibleSize();
    UILayer *lay = UILayer::create();
    addChild(lay);
    
    w = GUIReader::shareReader()->widgetFromJsonFile("gui/ballUI_3_0.json");
    lay->addWidget(w);
    w->setSize(size);
    
    //CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(ChatView::onPauseGame), "pauseGame", NULL);
    
    
    timeNow = static_cast<Label*>(UIHelper::seekWidgetByName(w, "time"));
    passTime = 0;
    
    //正在录音
    inRecord = static_cast<Layout*>(UIHelper::seekWidgetByName(w, "Panel_40"));
    inRecord->setVisible(true);
    inRecord->setEnabled(false);
    
    ani = static_cast<ImageView*>(UIHelper::seekWidgetByName(inRecord, "ani"));
    CCLog("ani name is %s", ani->getName());
    CCAnimation *an;
    an = CCAnimationCache::sharedAnimationCache()->animationByName("speakAni");
    
    //ImageView 本身是 Widget 需要其render来绘制动画的
    if(an == NULL) {
        an = CCAnimation::create();
        
        //CCArray *arr = CCArray::create();
        char buf[256];
        for (int i = 1;  i <= 6; i++) {
            sprintf(buf, "gui/img_talk%d.png", i);
            an->addSpriteFrameWithFileName(buf);
        }
        
        an->setDelayPerUnit(0.1);
        an->setRestoreOriginalFrame(true);
        CCLog("ani dura %f", an->getDuration());
        
        
        //an =  CCAnimation::create(arr, 0.1, true);
        CCAnimationCache::sharedAnimationCache()->addAnimation(an, "speakAni");
    }
    
    an = CCAnimationCache::sharedAnimationCache()->animationByName("myVoiceAni");
    if (an == NULL) {
        an = CCAnimation::create();
        char buf[256];
        for(int i=1; i <= 3; i++) {
            sprintf(buf, "gui/img_Chat_VoiceIconRight%d.png", i);
            an->addSpriteFrameWithFileName(buf);
        }
        an->setDelayPerUnit(0.2);
        an->setRestoreOriginalFrame(true);
        CCAnimationCache::sharedAnimationCache()->addAnimation(an, "myVoiceAni");
    }
    
    
    an = CCAnimationCache::sharedAnimationCache()->animationByName("otherVoiceAni");
    if (an == NULL) {
        an = CCAnimation::create();
        char buf[256];
        for (int i=1; i <= 3; i++) {
            sprintf(buf, "gui/img_Chat_VoiceIconLeft%d.png", i);
            an->addSpriteFrameWithFileName(buf);
        }
        an->setDelayPerUnit(0.2);
        an->setRestoreOriginalFrame(true);
        CCAnimationCache::sharedAnimationCache()->addAnimation(an, "otherVoiceAni");
    }
    
    
    
    inImage = static_cast<ImageView*>(UIHelper::seekWidgetByName(inRecord, "inRecord"));
    inImage->setEnabled(false);
    cancelImg = static_cast<ImageView*>(UIHelper::seekWidgetByName(inRecord, "cancel"));
    cancelImg->setEnabled(true);
    bigImg = (ImageView*)(UIHelper::seekWidgetByName(inRecord, "showBig"));
    
    midView = static_cast<Layout*>(UIHelper::seekWidgetByName(w, "Panel_4"));
    oldSizePer = midView->getSizePercent();
    
    
    //Match *minf = Logic::getInstance()->matchInfo;
    
    
    
    scheduleUpdate();
    return true;
}

void ChatView::closeChat() {
    if (closeYet) {
        return;
    }
    if (CCDirector::sharedDirector()->getNextScene() != NULL) {
        return;
    }
    Logic::getInstance()->inChatRoom = false;
    
    
    //Logic::getInstance()->clearMsg();
    
    closeYet = true;
    CCLog("back start listen channel");
    tf->closeIME();
    //关闭频道监听
    startReceiveRedis(-1);
    
    //发出一条离开聊天室的消息
    string emp;
    bool suc = m_channel->sendMessage(emp, Channel::QUIT_TYPE, -1);
    
    //关闭发送redis 连接
    //closeSendRedisC();
    startSendRedis(-1);
    
    
    //只有在关闭 聊天界面的时候 才会取消 pauseGame的响应
    //CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "pauseGame");
    
    CCDirector::sharedDirector()->popSceneWithTransition(transTime);
    
    Logic::getInstance()->clearVid();
    Logic::getInstance()->clearImageId();
    
    //退出聊天室就无法接受回调了可以交给Logic来处理
    
    HttpModel *hm = HttpModel::getInstance();
    UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
    User *user = us->getUser();
    std::map<string, string> postData;
    //char buf[256];
    //sprintf(buf, "%d", Logic::getInstance()->getUID());
    //sprintf(buf, "%d", user->uid);
    
    CCLog("exit chat channel %d %d", user->uid, m_matchID);
    
    char buf[256];
    sprintf(buf, "%d", user->uid);
    postData["userid"] = buf;
    
    //
    //postData["cid"] = buf;
    
    //postData["uid"] = Logic::getInstance()->getUID();
    //postData["cid"] = Logic::getInstance()->getCID();
    
    
    //退出聊天室
    sprintf(buf, "match/%d", m_matchID);
    hm->addRequest(buf, "PUT", postData, NULL, NULL, NULL);
    
    
    CCLog("chat view start receive redis");
    //关闭监听器 频道
    //startReceiveRedis(-1);
}

void ChatView::onBack(cocos2d::CCObject *obj, TouchEventType tt){
    //未初始化完成 不能 点击按钮
    
    CCLog("chatView stata is %d", state);
    //if (state != 2) {
    //    return;
    //}
    switch (tt) {
        case cocos2d::ui::TOUCH_EVENT_BEGAN:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_MOVED:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_ENDED:
        {
            closeChat();
        }
            
            
            break;
        case cocos2d::ui::TOUCH_EVENT_CANCELED:
        {
            
        }
            break;
        default:
            break;
    }
}



//点击listView 则 关闭键盘 detachIME
void ChatView::onBackground(cocos2d::CCObject *obj, ListViewEventType tt){
    CCLog("background touch event %d", tt);
    switch (tt) {
        case cocos2d::ui::LISTVIEW_ONSELECTEDITEM_START:
        {
            CCLog("touch background");
            tf->closeIME();
        }
            break;
        case cocos2d::ui::LISTVIEW_ONSELECTEDITEM_END:
        {
            
        }
            break;
        default:
            break;
    }
}
void ChatView::onBackgroundTouch(cocos2d::CCObject *obj, TouchEventType tt){
    switch (tt) {
        case cocos2d::ui::TOUCH_EVENT_BEGAN:
        {
            tf->closeIME();
        }
            break;
        default:
            break;
    }
}

void ChatView::onAdd(cocos2d::CCObject *obj, TouchEventType tt){
    switch (tt) {
        case cocos2d::ui::TOUCH_EVENT_BEGAN:
        {
            tf->closeIME();
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_MOVED:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_ENDED:
        {
            //打开了选择图片页面
            selectImgYet = true;
            openImageSelect();
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_CANCELED:
        {
            
        }
            break;
        default:
            break;
    }
}

void ChatView::onChatInfo(CCObject *obj, TouchEventType tt){
    //if (state != 2) {
    //    return;
   // }
    
    switch (tt) {
        case cocos2d::ui::TOUCH_EVENT_BEGAN:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_MOVED:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_ENDED:
        {
            CCScene *nxt = CCDirector::sharedDirector()->getNextScene();
            if (nxt == NULL) {
                tf->closeIME();
                //CCScene *tf = CCTransitionSlideInR::create(1, ChatInfo::scene());
                //if (curInScene) {
                
                CCDirector::sharedDirector()->pushScene(getTransScene(ChatInfo::scene()));
            }
            //}
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_CANCELED:
        {
            
        }
            break;
        default:
            break;
    }
}

void ChatView::onOtherVoice(cocos2d::CCObject *obj,  TouchEventType tt){
    CCLog("on Other Voice");
    switch (tt) {
        case cocos2d::ui::TOUCH_EVENT_BEGAN:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_MOVED:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_ENDED:
        {
            //先播放 清理之前正在播放的动画  playVoice 接着再 设置 playingVoice
            Button *n = static_cast<Button*>(obj);
            int vd = n->getTag();
            ImageView *voiceAni = static_cast<ImageView*>(UIHelper::seekWidgetByName(n, "voiceAni"));
            if (playingVoice == voiceAni) {
                playVoice(-1);
            }else {
                playVoice(vd);
                
                voiceAni->getRender()->runAction(CCRepeatForever::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("otherVoiceAni"))));
                playingVoice = voiceAni;
                dframe = voiceAni->getRender()->displayFrame();
                dframe->retain();
            }
        }
            
            break;
        case cocos2d::ui::TOUCH_EVENT_CANCELED:
        {
            
        }
            break;
        default:
            break;
    }
}
void ChatView::onSay(cocos2d::CCObject *obj, TouchEventType tt){
    switch (tt) {
        case cocos2d::ui::TOUCH_EVENT_BEGAN:
        {
            if (!inVoice) {
                tf->closeIME();
            }
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_MOVED:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_ENDED:
        {
            if (!inVoice) {
                inVoice = true;
                //sayWord->setTitleText("Word");
                sayWord->loadTextures("gui/btn_Chat_Keyboard_Nor.png", "gui/btn_Chat_Keyboard_Press.png", "");
                
                tf->setEnabled(false);
                speak->setEnabled(true);
                inputBorder->setEnabled(false);
            }else{
                inVoice = false;
                //sayWord->setTitleText("Voice");
                sayWord->loadTextures("gui/btn_Chat_Voice_Nor.png", "gui/btn_Chat_Voice_Press.png", "");
                tf->setEnabled(true);
                speak->setEnabled(false);
                inputBorder->setEnabled(true);
            }
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_CANCELED:
        {
            
        }
            break;
        default:
            break;
    }
}


string ChatView::getVoiceTime(int vl) {
    char buf[512];
    int smh[3];
    smh[0] = vl%60;
    vl /= 60;
    smh[1] = vl%60;
    vl /= 60;
    smh[2] = vl%60;
    
    bool notZero = false;
    string rt;
    for (int i = 2; i > -1; i--) {
        if (notZero || smh[i] > 0 || i == 0) {
            notZero = true;
            sprintf(buf, "%d\"", smh[i]);
            rt += buf;
        }
    }
    CCLog("length voice %s", rt.c_str());
    return rt;
}

float ChatView::getVoiceWidth(float vl, Widget *voice) {
    //max Width 最宽宽度
    CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
    
    float lwid = fs.width-114-120;
    if (vl >= 5) {
        voice->setSize(CCSizeMake(lwid, 70));
    }else if(vl >= 3) {
        voice->setSize(CCSizeMake(std::max(lwid/2, 110.0f), 70));
    }else if(vl >= 1) {
        voice->setSize(CCSizeMake(100, 70));
    }
    
    //voice->setSize(CCSizeMake(150, 70));
    return 0;
}

//停止录音转入发送流程
void ChatView::updateRecordTime(float dt) {
    if (isRecording) {
        float totRTime = 60;
        
        recordTime += dt;
        if (totRTime-recordTime <= 9) {
            overTime->setEnabled(true);
            overTime->setVisible(true);
            char buf[128];
            sprintf(buf, "%d", (int)std::max(0.0f, totRTime-recordTime));
            overTime->setText(buf);
        }
        if (recordTime >= totRTime) {
            stopRecordAndSend();
        }
    }
}
//超过60s 停止并且发送语音
void ChatView::stopRecordAndSend() {
    if (!isRecording) {
        return;
    }
    
    isRecording = false;
    overTime->setEnabled(false);
    
    recordTime = 0;
    
    stopRecord();
    inRecord->setEnabled(false);
    string fn = getFileName();
    CCLog("file name %s", fn.c_str());
    int vl = getVoiceLength(fn.c_str());
    CCLog("getVoiceLength %d", vl);
    
    //录音时间太短了
    if (vl < 1) {
        CCMessageBox("录音时间太短", "提醒");
        //stopRecord();
        
        inRecord->setEnabled(false);
        inImage->setEnabled(false);
        cancelImg->setEnabled(false);
        
        
        Logic::getInstance()->resetVid();
        speak->setTitleText("按住说话");
        ani->getRender()->stopAllActions();
        return;
    }
    
    
    
    CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
    
    CCSize ws = myvImg->getSize();
    CCSize hsz = head2->getSize();
    float height = std::max(ws.height, hsz.height);
    height += marginHeight;
    //voice2->setEnabled(true);
    //lab2->setEnabled(false);
    
    char buf[512];
    UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
    User *user = us->getUser();
    sprintf(buf, "flags/%d.png", user->flagId);
    
    
    
    UIPanel *pan = static_cast<UIPanel*>(myvoice->clone());
    Button *voice = static_cast<Button*>(UIHelper::seekWidgetByName(pan, "voice2"));
    ImageView *mhead = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "head3"));
    mhead->loadTexture(buf);
    
    
    ImageView *error = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "error"));
    
    //声音文件编号 当前录制的音频文件编号
    voice->setTag(getMyRecordVid());
    voice->addTouchEventListener(this, toucheventselector(ChatView::onVoice));
    
    
    //stopRecord();
    
    
    
    
    Label *vt = static_cast<Label*>(UIHelper::seekWidgetByName(pan, "voiceTime"));
    
    CCLog("stop record get FileName");
    
    getVoiceWidth(vl, voice);
    
    string rt;
    rt = getVoiceTime(vl);
    CCLog("length voice %s", rt.c_str());
    vt->setText(rt);
    
    
    
    
    pan->setEnabled(true);
    pan->setSize(CCSizeMake(fs.width, height));
    pan->setSizeType(SIZE_ABSOLUTE);
    pan->setVisible(true);
    lv->pushBackCustomItem(pan);
    
    
    trySendMsg(fn, Channel::VOICE_TYPE, error);
    
    
    
    speak->setTitleText("按住说话");
    
    ani->getRender()->stopAllActions();
}

void ChatView::onSpeak(cocos2d::CCObject *obj, TouchEventType tt){
    CCLog("onSpeak event %d", tt);
    
    switch (tt) {
        case cocos2d::ui::TOUCH_EVENT_BEGAN:
        {
            recordTime = 0;
            isRecording = true;
            
            //说话关闭键盘
            //tf->closeIME();
            
            playVoice(-1);
            startRecord();
            speak->setTitleText("松开结束");
            inRecord->setEnabled(true);
            inImage->setEnabled(true);
            inImage->setVisible(true);
            
            cancelImg->setEnabled(false);
            inImage->getRender()->runAction(CCFadeIn::create(0.5));
            bigImg->setEnabled(false);
            CCAnimation *an = CCAnimationCache::sharedAnimationCache()->animationByName("speakAni");
            CCLog("getAnimation %x", an);
            CCAnimate *animate = CCAnimate::create(an);
            CCLog("animate %f", animate->getDuration());
            
            ani->getRender()->runAction(CCRepeatForever::create(animate));
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_MOVED:
        {
            //移动出屏幕空间的时候
            Button *but = (Button*)obj;
            bool inT = but->isFocused();
            if (!inT) {
                inImage->setEnabled(false);
                cancelImg->setEnabled(true);
                cancelImg->setVisible(true);
            }else {
                inImage->setEnabled(true);
                cancelImg->setEnabled(false);
            }
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_ENDED:
        {
            //isRecord = false;
            stopRecordAndSend();
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_CANCELED:
        {
            if (isRecording) {
                isRecording = false;
                overTime->setEnabled(false);
                
                
                inRecord->setEnabled(false);
                inImage->setEnabled(false);
                cancelImg->setEnabled(false);
                
                stopRecord();
                Logic::getInstance()->resetVid();
                speak->setTitleText("按住说话");
                
                ani->getRender()->stopAllActions();
            }
        }
            break;
        default:
            break;
    }
}
//播放自己的声音  别人的声音是 onOthervoice
void ChatView::onVoice(cocos2d::CCObject *obj, TouchEventType tt){
    switch (tt) {
        case cocos2d::ui::TOUCH_EVENT_BEGAN:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_MOVED:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_ENDED:
        {
            Button *n = static_cast<Button*>(obj);
            int vd = n->getTag();
            
            ImageView *voiceAni = static_cast<ImageView*>(UIHelper::seekWidgetByName(n, "voiceAni"));
            //当前正在播放的 语音 是 当前语音 则 停止
            //停止当前语音
            if (playingVoice == voiceAni) {
                playVoice(-1);
                return;
            }
            
            
            playVoice(vd);
            
            voiceAni->getRender()->runAction(CCRepeatForever::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("myVoiceAni"))));
            
            //play Voice Finish
            //voiceMap->setObject(voiceAni, vd);
            playingVoice = voiceAni;
            dframe = voiceAni->getRender()->displayFrame();
            dframe->retain();
        }
            
            break;
        case cocos2d::ui::TOUCH_EVENT_CANCELED:
        {
            
        }
            break;
        default:
            break;
    }
}
void ChatView::onSend(cocos2d::CCObject *obj, TouchEventType tt){
    switch (tt) {
        case cocos2d::ui::TOUCH_EVENT_BEGAN:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_MOVED:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_ENDED:
        {
            std::string text = tf->getStringValue();
            //统计字符数量
            CCLog("send Msg %d", text.length());
            
            tf->setText("");
            tf->setPosition(oldPos);
            
            //发送之后调整高度
            CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
            //float nw = 400*fs.width/640;
            //tf->setSize(CCSizeMake(nw, 58));
            
            
            //content->setTitleText(text);
            //word->setText(text);
            
            
            float lwid = getLabelWidth();
            //调整尺寸
            
            testLabel->setString(text.c_str());
            //first test width
            testLabel->setDimensions(CCSize(0, 0));
            CCSize osz = testLabel->getContentSize();
            //CCSize tsz;
            if(osz.width > lwid){
                testLabel->setDimensions(CCSize(lwid, 0));
            }
            
            
            //宽度存在一个bug 需要+1 才能显示完整的文字 高度也可能存在这个浮点数bug
            CCSize tsz = testLabel->getContentSize();
            //tsz.width += 3;
            if (tsz.width > lwid) {
                tsz.width = lwid;
            }
            tsz.height += 2;
            CCLog("testlabel Size %f %f", tsz.width, tsz.height);
            
            lab2->setText("");
            lab2->setSize(tsz);
            //CCLog("setText %s", text.c_str());
            lab2->setText(text);
            
            CCLog("size is what %f %f %f %f",osz.width, osz.height, tsz.width, tsz.height);
            
            CCSize ws = lab2->getSize();
            CCSize hsz = head2->getSize();
            
   
            
            UIPanel *pan = static_cast<UIPanel*>(twoWord->clone());
            ImageView *head2 = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "head2"));
            ImageView *chatBack = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "chatBack"));
            //高度至少大于70 才没有明显的缺口 背景框
            chatBack->setSize(CCSizeMake(std::max(100.0f, ws.width+40), MAX(ws.height+textMargin, 70)));
            
            CCSize cbsize = chatBack->getSize();
            float height = std::max(cbsize.height, hsz.height);
            height += marginHeight;
            
            char buf[512];
            
            UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
            User *user = us->getUser();
            sprintf(buf, "flags/%d.png", user->flagId);
            head2->loadTexture(buf);
            
            
            pan->setEnabled(true);
            pan->setSize(CCSizeMake(fs.width, height));
            pan->setSizeType(SIZE_ABSOLUTE);
            pan->setVisible(true);
            
            lv->pushBackCustomItem(pan);
            
            //保持指针传递或者传递string更安全
            
            //base64 encode
            int olen;
            char *odata = base64_encode((const unsigned char*)text.c_str(), (size_t)text.size(), (size_t*)&olen);
            std::string od = string(odata, olen);
            free(odata);
            
            ImageView *error = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "error"));
            trySendMsg(od, Channel::TEXT_TYPE, error);
            
            adjustBut();
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_CANCELED:
        {
            
        }
            break;
        default:
            break;
    }
}

//使用redis的 超时机制来注册用户的频道怎么样呢？
void ChatView::onEnterChat(bool isSuc, string s, void *param){
    CCLog("enter Chat suc %s", s.c_str());
    
}

void ChatView::updateEnter(float diff){
    //更新服务器上面 的用户在线状态
    if (!enterChatYet) {
        lastUpdateTime = 60;
        //进入房间释放一次纹理缓存空间
        CCTextureCache::sharedTextureCache()->purgeSharedTextureCache();
        
        enterChatYet = true;
        HttpModel *hm = HttpModel::getInstance();
        std::map<string, string> postData;
        //char buf[256];
        //sprintf(buf, "%d", Logic::getInstance()->getUID());
        
        UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
        User *user = us->getUser();
        
        char buf[256];
        sprintf(buf, "%d", user->uid);
        postData["userid"] = buf;
        
        //
        //postData["cid"] = buf;
        
        
        //hm->addRequest("enterChannel", "POST", postData, this, MYHTTP_SEL(ChatView::onEnterChat), NULL);
        
        
        
        
        
        CCLog("enter chat channel %d", m_channel->cid);
        sprintf(buf, "match/%d", m_channel->cid);
        //初始化频道消息缓存
        //ChannelService::getInstance()->initChannel(Logic::getInstance()->getCID());
        
        hm->addRequest(buf, "POST", postData, this, MYHTTP_SEL(ChatView::onEnterChat), NULL);
    } else {
        //更新数据
        lastUpdateTime -= diff;
        if (lastUpdateTime <= 0) {
            enterChatYet = false;
        }
    }
}


//没有保存msg对象 直接展示了msg对象了
//获取服务器 发送的 uid 消息对象的时候
void ChatView::onMsg(bool isSuc, std::string s, void *param) {
    //只可以获取5分钟内的消息 从当前时间开始算起
    state = 2;
    if (isSuc) {
        insertHistory = true;
        //避免太多的聊天历史记录
        CCLog("getHistorymessage suc %d", s.length());
        rapidjson::Document d;
        d.Parse<0>(s.c_str());
        if (d["state"].GetInt() == 0) {
            return;
        }
        
        
        //CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
        //float lwid = fs.width-114-10-30;
        //每行14个汉字 每个 30  520 长度
        //float lwid = 475;
        
        //UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
        //User *user = us->getUser();
        
        Message msg;
        //消息长度太长就不要 前面的历史消息了 显示不超过4条历史消息
        int ord = 0;
        //10 条历史记录 最多10条
        int start = std::max(0.0f, d["data"].Size()-10.0f);
        
        lv->setTouchEnabled(false);
        
        for(int i =start; i <d["data"].Size(); i++) {
            const rapidjson::Value &b = d["data"][i];
            msg.setData(b);
            //消息插入到头部
            if (inUpdate) {
                insertMessage(&msg, ord);
            //插入消息到尾部
            }else {
                insertMessage(&msg, -1);
            }
            ord++;
        }
        lv->setTouchEnabled(true);
        
        //第一次获取历史记录 到 页面底端
        if (firstGetMsg) {
            needScroll = 1;
        }
        
        insertHistory = false;
    } else {
        CCLog("get history message fail");
        //正在更新历史消息 时间回滚一下
        //获取历史消息 没有监听历史消息频道
        if (inUpdate) {
            MessageService *ms = (MessageService*)ServiceCenter::getInstance()->getService(ServiceCenter::MESSAGE_SERVICE);
            ms->restoreHisTime();
        
        //初次重新获取历史消息 放弃这些历史消息
        } else {
            //state = 0;
        }
    }
    
    
    firstGetMsg = false;
    inUpdate = false;
}
float ChatView::getLabelWidth() {
    CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
    //float lwid = fs.width-114-90;
    return TEXT_WIDTH;
    
    //return lwid;
}
//插入历史消息不用 考虑是不是用户本身
void ChatView::insertMessage(Message *message, int ord) {
    CCLog("insert message is  %d", ord);
    
    int flagId = message->flagId;
    std::string ctype = message->mtype;
    std::string conText;
    //bool isText = false;
    
    UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
    User *user = us->getUser();
    
    CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
    float lwid = getLabelWidth();
    int sender = message->sender;
    string senderName = message->senderName;
    const char *text = senderName.c_str();
    char buf[512];
    int state = 0;
    int count = 0;
    int i;
    for (i = 0 ; i < senderName.size(); i++) {
        char value = text[i];
        if (state == 0) {
            if (value >= 0 && value <= 127) {
                state = 0;
                count++;
            }else {
                state = 1;
            }
        }else if(state == 1){
            state = 2;
        }else if(state == 2) {
            state = 0;
            count++;
        }
        buf[i] = value;
        if (count >= 4) {
            break;
        }
    }
    buf[++i] = '\0';
    
    if (count >= 4) {
        senderName = buf;
    }
    
    CCLog("count %d short name %s", count, senderName.c_str());
    
    
    //切割UTF 前几个字符
    //CCLog("name length %d", senderName.length());
    //限制字符串长度
    //senderName = senderName.substr(0, 4);
    
    CCLog("message sender is who %d %d", sender, user->uid);
    CCLog("sender name %s %d", senderName.c_str(), senderName.size());
    //testUtf8C(senderName.c_str());
    
    
    //自己发送的不要显示
    //显示发送者的头像数据 根据Id 获得相关头像
    //header Cache
    //调试关闭
    //注册一个新用户才能发出语音来
    if (!insertHistory) {
        if(sender == user->uid) {
            return;
        }
    }
    
    
    //插入我方的历史消息
    //自己发的 文字 历史消息
    //只有用户拖动 才插入 用户的历史消息
    //忽略自己的消息
    if (insertHistory && sender == user->uid) {
        //拖动更新 或者 第一次 获取 历史 消息 的时候 才插入 用户的自己的消息
        //inUpdate 和 所有消息数目 并非第一次获取历史消息
        //不是拖动获取 也不是 第一次获取
        if (!inUpdate && !firstGetMsg) {
            CCLog("not drag not first get %d %d", inUpdate, firstGetMsg);
            return;
        }
        
        
        if (ctype.compare("text") == 0) {
            CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
            //float lwid = getLabelWidth();
            //调整尺寸
            unsigned char *tword = (unsigned char*)message->content.c_str();
            unsigned char *out;
            int outLen = cocos2d::base64Decode(tword, strlen((const char*)tword), &out);
            conText = string((const char*)out, (size_t)outLen);
            free(out);
            
            
            testLabel->setString(conText.c_str());
            //first test width
            testLabel->setDimensions(CCSize(0, 0));
            CCSize osz = testLabel->getContentSize();
            //CCSize tsz;
            if(osz.width > lwid){
                testLabel->setDimensions(CCSize(lwid, 0));
            }
            
            //宽度存在一个bug 需要+1 才能显示完整的文字 高度也可能存在这个浮点数bug
            CCSize tsz = testLabel->getContentSize();
            tsz.width++;
            tsz.height++;
            
            lab2->setText("");
            lab2->setSize(tsz);
            //CCLog("setText %s", text.c_str());
            lab2->setText(conText);
            
            CCLog("size is what %f %f %f %f",osz.width, osz.height, tsz.width, tsz.height);
            
            CCSize ws = lab2->getSize();
            CCSize hsz = head2->getSize();
            
            
            
            UIPanel *pan = static_cast<UIPanel*>(twoWord->clone());
            ImageView *head2 = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "head2"));
            ImageView *chatBack = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "chatBack"));
            //高度至少大于70 才没有明显的缺口 背景框
            chatBack->setSize(CCSizeMake(std::max(100.0f, ws.width+40), MAX(ws.height+textMargin, 70)));
            
            
            char buf[512];
            
            UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
            User *user = us->getUser();
            sprintf(buf, "flags/%d.png", user->flagId);
            head2->loadTexture(buf);
            
            CCSize cbSize = chatBack->getSize();
            float height = std::max(cbSize.height, hsz.height);
            height += marginHeight;
            
            pan->setEnabled(true);
            pan->setSize(CCSizeMake(fs.width, height));
            pan->setSizeType(SIZE_ABSOLUTE);
            pan->setVisible(true);
            
            //lv->pushBackCustomItem(pan);
            if (ord == -1) {
                lv->pushBackCustomItem(pan);
            }else {
                lv->insertCustomItem(pan, ord);
            }
        }else if(ctype.compare("voice") == 0) {
            unsigned char *voice = (unsigned char*)message->content.c_str();
            unsigned char *out;
            int outLen = cocos2d::base64Decode(voice, strlen((const char*)voice), &out);
            int vid = Logic::getInstance()->getVid();
            
            string fname = storeFile(out, outLen, vid);
            free(out);

            
            CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
            
            CCSize ws = myvImg->getSize();
            CCSize hsz = head2->getSize();
            float height = std::max(ws.height, hsz.height);
            height += marginHeight;
            
            char buf[512];
            UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
            User *user = us->getUser();
            sprintf(buf, "flags/%d.png", user->flagId);
            
            
            
            UIPanel *pan = static_cast<UIPanel*>(myvoice->clone());
            Button *voiceBtn = static_cast<Button*>(UIHelper::seekWidgetByName(pan, "voice2"));
            ImageView *mhead = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "head3"));
            mhead->loadTexture(buf);
            
            
            ImageView *error = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "error"));
            
            //声音文件编号 当前录制的音频文件编号
            voiceBtn->setTag(vid);
            voiceBtn->addTouchEventListener(this, toucheventselector(ChatView::onVoice));
            
            
            stopRecord();
            
            
            
            
            Label *vt = static_cast<Label*>(UIHelper::seekWidgetByName(pan, "voiceTime"));
            
            CCLog("stop record get FileName");
            
            /*
            string fn = getFileName();
            CCLog("file name %s", fn.c_str());
            
            CCLog("getVoiceLength %d", vl);
            
            */
            //本地分析文件长度
            int vl = getVoiceLength(fname.c_str());
            message->voiceTime = vl;
            
            getVoiceWidth(message->voiceTime, voiceBtn);
            string rt;
            rt = getVoiceTime(message->voiceTime);
            CCLog("length voice %s", rt.c_str());
            vt->setText(rt);
            
            pan->setEnabled(true);
            pan->setSize(CCSizeMake(fs.width, height));
            pan->setSizeType(SIZE_ABSOLUTE);
            pan->setVisible(true);
            
            if (ord == -1) {
                lv->pushBackCustomItem(pan);
            }else {
                lv->insertCustomItem(pan, ord);
            }
            
        } else if(ctype.compare("image") == 0) {
            unsigned char *image = (unsigned char*)message->content.c_str();
            unsigned char *out;
            int outLen = cocos2d::base64Decode(image, strlen((const char*)image), &out);
            
            
            char name[128];
            int imgId = Logic::getInstance()->getImgId();
            sprintf(name, "tmp_%d", imgId);
            const char*key = name;

            
            time_t st;
            time(&st);
            
            CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->textureForKey(key);
            CCImage *pImage = new CCImage();
            pImage->initWithImageData(out, outLen, CCImage::kFmtPng);
            
            pTexture = CCTextureCache::sharedTextureCache()->addUIImage(pImage, key);
            CC_SAFE_RELEASE(pImage);
            free(out);
            
            
            time_t et;
            time(&et);
            float pass = et-st;
            CCLog("load texture time %f", pass);
            
            
            CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
            //图片宽度需要进行缩放到100*100 的尺寸
            CCSize ws = CCSizeMake(100, 100);
            CCSize hsz = head->getSize();
            //float height = std::max(ws.height, hsz.height);
            //height += 20;
            
            
            UIPanel *pan = static_cast<UIPanel*>(myImg->clone());
            //Button *vimg = static_cast<Button*>(UIHelper::seekWidgetByName(pan, "voice2"));
            ImageView *img = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "img"));
            img->loadTexture(key, UI_TEX_TYPE_LOCAL);
            CCSize sz = img->getSize();
            CCLog("newImage Size %f %f %f %f %f %d", img->getContentSize().width, img->getContentSize().height, sz.width, sz.height, img->getScale(), img->getSizeType(), img->isIgnoreAnchorPointForPosition());
            img->setSize(img->getContentSize());
            
            ImageView *chatBack = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "chatBack"));
            chatBack->setSize(CCSizeMake(std::max(140.0f,img->getContentSize().width+imgWidthMargin), std::max(120.0f, img->getContentSize().height+imgHeightMargin)));
            
            
            
            //adjustScale(img, 200, 200);
            
            Button *onImg = static_cast<Button*>(UIHelper::seekWidgetByName(pan, "onImgBut"));
            onImg->addTouchEventListener(this, toucheventselector(ChatView::onImgBnt));
            onImg->setUserData(img);
            onImg->setVisible(false);
            onImg->setSize(chatBack->getSize());
            
            
            
            
            char buf[512];
            UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
            User *user = us->getUser();
            sprintf(buf, "flags/%d.png", user->flagId);
            
            ImageView *mhead = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "head2"));
            mhead->loadTexture(buf);
            
            ImageView *error = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "error"));
            
            
            
            float height = std::max(mhead->getSize().height, chatBack->getSize().height)+marginHeight;
            
            //使用customSize 而不是图片自动size
            img->ignoreContentAdaptWithSize(false);
            
            
            pan->setEnabled(true);
            pan->setSize(CCSizeMake(fs.width, height));
            pan->setSizeType(SIZE_ABSOLUTE);
            pan->setVisible(true);
            
            if (ord == -1) {
                lv->pushBackCustomItem(pan);
            }else {
                lv->insertCustomItem(pan, ord);
            }
        }
    
        //处理完自己 发送的 历史消息 之后 也要 删除旧的消息
        //消息太多就删除第一条记录
        //减少 初始化的消息
        //向上拖动 拉去 历史记录的时候 可能 有问题
        if (lv->getItems()->count() > 20) {
            lv->removeItem(0);
            //释放纹理存储空间
            CCTextureCache::sharedTextureCache()->purgeSharedTextureCache();
        }
        
        return;
    }
    
    
    
    UIPanel *pan = NULL;
    //后台只publish 一个索引信息 从服务器拉去 实际的语音讯息
    if (ctype.compare("text") == 0 ){
        unsigned char *tword = (unsigned char*)message->content.c_str();
        unsigned char *out;
        int outLen = cocos2d::base64Decode(tword, strlen((const char*)tword), &out);
        conText = string((const char*)out, (size_t)outLen);
        free(out);
        
        CCLog("read text is %s", conText.c_str());
        
        
        testLabel->setString(conText.c_str());
        //first test width
        testLabel->setDimensions(CCSize(0, 0));
        CCSize osz = testLabel->getContentSize();
        //CCSize tsz;
        if(osz.width > lwid){
            testLabel->setDimensions(CCSize(lwid, 0));
        }
        
        CCSize tsz = testLabel->getContentSize();
        tsz.width++;
        tsz.height++;
        
        lab->setText("");
        lab->setSize(tsz);
        lab->setText(conText);
        
        CCSize ws = lab->getSize();
        CCSize hsz = head->getSize();
        
        
        pan = static_cast<UIPanel*>(oneWord->clone());
        
        
        
        char buf[512];
        sprintf(buf, "flags/%d.png", flagId);
        ImageView *head = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "head"));
        head->loadTexture(buf);
        ImageView *chatBack = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "chatBack"));
        chatBack->setSize(CCSizeMake(std::max(100.0f, ws.width+40), MAX(ws.height+textMargin, 70)));
        
        
        float height = std::max(chatBack->getSize().height, hsz.height)+marginHeight;
        
        
        pan->setEnabled(true);
        pan->setSize(CCSizeMake(fs.width, height));
        pan->setSizeType(SIZE_ABSOLUTE);
        pan->setVisible(true);
        
        
        
        CCLog("push CutonItem where");
        if (ord == -1) {
            lv->pushBackCustomItem(pan);
        }else {
            lv->insertCustomItem(pan, ord);
        }
        
        
    } else if(ctype.compare("voice") == 0){
        //decode base64
        unsigned char *voice = (unsigned char*)message->content.c_str();
        unsigned char *out;
        int outLen = cocos2d::base64Decode(voice, strlen((const char*)voice), &out);
        int vid = Logic::getInstance()->getVid();
        
        string fname = storeFile(out, outLen, vid);
        free(out);
        
        CCSize ws = ovoice->getSize();
        CCSize hsz = ohead->getSize();
        float height = std::max(ws.height, hsz.height);
        height += marginHeight;
        
        //ovoice->setTag(vid);
        //vid++;
        
        pan = static_cast<UIPanel*>(otherVoice->clone());
        pan->setEnabled(true);
        pan->setSize(CCSizeMake(fs.width, height));
        pan->setSizeType(SIZE_ABSOLUTE);
        pan->setVisible(true);
        
        
        
        
        char buf[512];
        //UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
        //User *user = us->getUser();
        sprintf(buf, "flags/%d.png", flagId);
        
        ImageView *mhead = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "ohead"));
        mhead->loadTexture(buf);
        
        
        Label *vt = static_cast<Label*>(UIHelper::seekWidgetByName(pan, "voiceTime"));
        
        
        int vl = getVoiceLength(fname.c_str());
        message->voiceTime = vl;
        //setVoice time according to real file length not according to message information
        
        string rt = getVoiceTime(message->voiceTime);
        vt->setText(rt);
        
        
        Button *newVoice = static_cast<Button*>(UIHelper::seekWidgetByName(pan, "ovoice"));
        newVoice->addTouchEventListener(this, toucheventselector(ChatView::onOtherVoice));
        newVoice->setTag(vid);
        
        getVoiceWidth(message->voiceTime, newVoice);
        
        //vid++;
        
        //lv->pushBackCustomItem(pan);
        if (ord == -1) {
            lv->pushBackCustomItem(pan);
        }else {
            lv->insertCustomItem(pan, ord);
        }
        
        
    }else if(ctype.compare("image") == 0) {
        
        unsigned char *image = (unsigned char*)message->content.c_str();
        unsigned char *out;
        int outLen = cocos2d::base64Decode(image, strlen((const char*)image), &out);
        
        
        char name[128];
        int imgId = Logic::getInstance()->getImgId();
        sprintf(name, "tmp_%d", imgId);
        const char*key = name;
        
        CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->textureForKey(key);
        CCImage *pImage = new CCImage();
        pImage->initWithImageData(out, outLen, CCImage::kFmtPng);
        
        pTexture = CCTextureCache::sharedTextureCache()->addUIImage(pImage, key);
        CC_SAFE_RELEASE(pImage);
        
        //int vid = Logic::getInstance()->getVid();
        //storeFile(out, outLen, vid);
        free(out);
        
        //CCSize ws = CCSizeMake(100, 100);
        CCSize hsz = ohead->getSize();
        //float height = std::max(ws.height, hsz.height);
        //height += 20;
        
        
        
        //ovoice->setTag(vid);
        //vid++;
        
        pan = static_cast<UIPanel*>(otherImg->clone());
    
        
        ImageView *img = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "img"));
        img->loadTexture(key, UI_TEX_TYPE_LOCAL);
        img->setSize(img->getContentSize());
        ImageView *chatBack = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "chatBack"));
        chatBack->setSize(CCSizeMake(std::max(140.0f, img->getContentSize().width+imgWidthMargin), std::max(120.0f, img->getContentSize().height+imgHeightMargin)));
        
        
        float height = std::max(hsz.height, chatBack->getSize().height)+marginHeight;
        pan->setEnabled(true);
        pan->setSize(CCSizeMake(fs.width, height));
        pan->setSizeType(SIZE_ABSOLUTE);
        pan->setVisible(true);
        
        
        //adjustScale(img, 100, 100);
        Button *onImg = static_cast<Button*>(UIHelper::seekWidgetByName(pan, "onImgBut"));
        onImg->addTouchEventListener(this, toucheventselector(ChatView::onImgBnt));
        onImg->setUserData(img);
        onImg->setVisible(false);
        onImg->setSize(chatBack->getSize());
        
        
        
        char buf[512];
        //UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
        //User *user = us->getUser();
        
        sprintf(buf, "flags/%d.png", flagId);
        
        ImageView *mhead = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "head"));
        mhead->loadTexture(buf);
        
        
        //lv->pushBackCustomItem(pan);
        if (ord == -1) {
            lv->pushBackCustomItem(pan);
        }else {
            lv->insertCustomItem(pan, ord);
        }
        
    }
    
    if (pan != NULL) {
        TextField *name = static_cast<TextField*>(UIHelper::seekWidgetByName(pan, "name"));
        if (name != NULL) {
            name->setMaxLengthEnabled(false);
            name->setText(senderName);
            name->setTouchEnabled(false);
            
        }
    }
    
    
    //lv->jumpToBottom();
    //lv->scrollToBottom(0.1, false);
    
    //向上拖动的时候 不需要 scroll
    if (!inUpdate) {
        needScroll = 1;
    }
    
    //lv->jumpToTop();
    
    CCSize insz = lv->getInnerContainerSize();
    CCSize sz = lv->getSize();
    CCLog("in size out size %f %f %f %f", insz.width, insz.height, sz.width, sz.height);
    
    
    
    //消息太多就删除第一条记录
    //减少 初始化的消息
    //向上拖动 拉去 历史记录的时候 可能 有问题
    if (lv->getItems()->count() > 20) {
        lv->removeItem(0);
        //释放纹理存储空间
        CCTextureCache::sharedTextureCache()->purgeSharedTextureCache();
    }
    
}


void ChatView::onInRecord(cocos2d::CCObject *obj, TouchEventType tt) {
    CCLog("onInRecord close recordView");
    switch (tt) {
        case cocos2d::ui::TOUCH_EVENT_BEGAN:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_MOVED:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_ENDED:
        {
            inRecord->setEnabled(false);
            bigImg->setEnabled(false);
            inRecord->addTouchEventListener(NULL, NULL);
            
        }
            
            break;
        case cocos2d::ui::TOUCH_EVENT_CANCELED:
        {
            
        }
            break;
        default:
            break;
    }
}

void ChatView::onImgBnt(cocos2d::CCObject *but, TouchEventType tt) {
    CCLog("on Image Button");
    switch (tt) {
        case cocos2d::ui::TOUCH_EVENT_BEGAN:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_MOVED:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_ENDED:
        {
            Button *b = (Button*)but;
            ImageView *im = (ImageView*)b->getUserData();
            bigImg->loadTexture(im->getFileName().c_str(), UI_TEX_TYPE_LOCAL);
            CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
            adjustScale(bigImg, fs.width-50, fs.height-50);
            im->setEnabled(true);
            bigImg->setEnabled(true);
            bigImg->setVisible(true);
            
            inImage->setEnabled(false);
            inImage->setVisible(false);
            
            cancelImg->setEnabled(false);
            cancelImg->setVisible(false);
            
            inRecord->addTouchEventListener(this, toucheventselector(ChatView::onInRecord));
            CCLog("show BigImage");
            inRecord->setEnabled(true);
            
            
        }
            
            break;
        case cocos2d::ui::TOUCH_EVENT_CANCELED:
        {
            
        }
            break;
        default:
            break;
    }
}

//重练 获取历史消息 忽略掉 自己的消息
void ChatView::reconnectGetHistory( long long lostTime) {
    state = 1;
    //断开接收连接 之后 重新获取历史消息记录
    CCLog("重新连接 获取 服务器数据？");
    
    char buf[512];
    long long startTime, endTime;
    
    //当前时间
    time_t td;
    time(&td);
    //从之前 到 当前时间
    startTime = lostTime;
    endTime = td+10;
    long long pass5 = endTime-5*60;
    //不能超过5分钟获取历史消息
    startTime = pass5 <  startTime? startTime: pass5;
    
    
    startTime *= 1000;
    endTime *= 1000;
    
    sprintf(buf, "message/%d/%lld/%lld", m_channel->cid, startTime, endTime);
    std::map<string, string> postData;
    
    HttpModel *hm = HttpModel::getInstance();
    hm->addRequest(buf, "GET", postData, this, MYHTTP_SEL(ChatView::onMsg), NULL);
}

//网络请求的时候要保存对象 直到请求结束才摧毁对象 从redis服务器接收请求
void ChatView::receiveMsg(){
    //接收该频道的消息
    if (state == 0) {
        state = 1;
        //等待频道初始化历史消息结束
        //缓存频道消息
        
        //初始化频道历史消息
        //出吃书获取 firstGetMsg
        firstGetMsg = true;
        MessageService *ms = (MessageService*)ServiceCenter::getInstance()->getService(ServiceCenter::MESSAGE_SERVICE);
        ms->getHistoryMessage(m_channel->cid, this, MYHTTP_SEL(ChatView::onMsg), true);
        
        
    //从redis 服务器接收数据
    }else if (state == 2) {
        //只有未初始化过receive 才获取 消息数据  在场景中才执行
        if (receive == NULL && !initReceiveYet && isRunning()) {
            CCLog("initReceiveYet false initial receive");
            //ChannelService::getInstance()->listenChannel();
            //MessageService *msg = (MessageService*)ServiceCenter::getInstance()->getService(ServiceCenter::MESSAGE_SERVICE);
            
            CCLog("channel listenChannel here when receive == NULL");
            initReceiveYet = true;
            //m_channel->listenChannel();
            startReceiveRedis(m_channel->cid);
            startSendRedis(m_channel->cid);
            
            //startReceiveRedis();
            
            
            CCLog("receive %x", receive);
        }else if(receive != NULL){
            
            long long lostTime;
            bool needUpdate = getLostTime(&lostTime);
            //断开连接需要获取历史记录
            if (needUpdate) {
                reconnectGetHistory(lostTime);
                return;
            }
            
            
            //redis 获取的用户名 需要base64 解码
            //http 获取的历史消息不需要
            Message *message = m_channel->refreshMessage();
            
            if(message){
                CCLog("refresh Message %x", message);
                //CCLog("read data %s %d", channel.c_str(), content.size());
                insertMessage(message, -1);
                //释放内存
                delete message;
            }
        }
    }
}
void ChatView::trySendMsg(string &con, Channel::MESSAGE_TYPE mt, ImageView *error) {
    //反馈界面 则通过feedback发送数据
    //if (m_channel->cid == FEEDBACK) {
        
    //}
    int msgId = Logic::getInstance()->getMsgId();
    bool suc = m_channel->sendMessage(con, mt, msgId);
    
    CCLog("trySendMsg ok %d", suc);
    //加入队列成功才会 检测是否发送成功
    if (suc) {
        errorMsg->addObject(error);
        error->setTag(msgId);
    }
    
    
    if (!suc) {
        error->setVisible(true);
    }else
        error->setVisible(false);
    
    //lv->jumpToTop();
    
    //lv->jumpToBottom();
    //lv->startAutoScrollChildrenWithDestination(CCPointMake(0, 0), 0.2, false);
    //lv->scrollToBottom(0.1, false);
    
    //向上拖动获取历史消息 不需要 scroll了
    
    needScroll = 1;
    
    CCSize insz = lv->getInnerContainerSize();
    CCSize sz = lv->getSize();
    CCLog("trySendMsg in size out size %f %f %f %f", insz.width, insz.height, sz.width, sz.height);
    CCLog("inner pos %f %f", lv->getInnerContainer()->getPosition().x, lv->getInnerContainer()->getPosition().y);
}
void ChatView::sendImg(){
    //打开过图片选择 并且 确认 获取图片 数据了
    if (selectImgYet && checkGetYet()) {
        selectImgYet = false;
        int len;
        void *data = getImage(&len);
        CCLog("get image ok select Img %x %d", data, len);
        //获取了本地图像数据
        if (data != NULL) {
            //发送出去显示为一张对话的图片
            
            char name[128];
            sprintf(name, "tmp_%d", Logic::getInstance()->getImgId());
            const char *key = name;
            
            struct timeval st;
            gettimeofday(&st, NULL);
            
            CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->textureForKey(key);
            CCImage *pImage = new CCImage();
            pImage->initWithImageData(data, len, CCImage::kFmtPng);
            
            pTexture = CCTextureCache::sharedTextureCache()->addUIImage(pImage, key);
            CC_SAFE_RELEASE(pImage);
            
            struct timeval et;
            gettimeofday(&et, NULL);
            double ett = et.tv_sec+(double)et.tv_usec/1000000.0;
            double stt = st.tv_sec+(double)st.tv_usec/1000000.0;
            float pass = ett-stt;
            CCLog("load texture time %f", pass);
            
            
            CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
            //图片宽度需要进行缩放到100*100 的尺寸
            CCSize ws = CCSizeMake(100, 100);
            CCSize hsz = head->getSize();
            //float height = std::max(ws.height, hsz.height);
            //height += 20;
            
            
            UIPanel *pan = static_cast<UIPanel*>(myImg->clone());
            //Button *vimg = static_cast<Button*>(UIHelper::seekWidgetByName(pan, "voice2"));
            ImageView *img = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "img"));
            img->loadTexture(key, UI_TEX_TYPE_LOCAL);
            CCSize sz = img->getSize();
            CCLog("newImage Size %f %f %f %f %f %d", img->getContentSize().width, img->getContentSize().height, sz.width, sz.height, img->getScale(), img->getSizeType(), img->isIgnoreAnchorPointForPosition());
            img->setSize(img->getContentSize());
            
            ImageView *chatBack = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "chatBack"));
            chatBack->setSize(CCSizeMake(std::max(140.0f,img->getContentSize().width+imgWidthMargin), std::max(120.0f, img->getContentSize().height+imgHeightMargin)));
            
            
            
            //adjustScale(img, 200, 200);
            
            Button *onImg = static_cast<Button*>(UIHelper::seekWidgetByName(pan, "onImgBut"));
            onImg->addTouchEventListener(this, toucheventselector(ChatView::onImgBnt));
            onImg->setUserData(img);
            onImg->setVisible(false);
            onImg->setSize(chatBack->getSize());
            
            
            
            
            char buf[512];
            UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
            User *user = us->getUser();
            sprintf(buf, "flags/%d.png", user->flagId);
            
            ImageView *mhead = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "head2"));
            mhead->loadTexture(buf);
            
            ImageView *error = static_cast<ImageView*>(UIHelper::seekWidgetByName(pan, "error"));
            
            
            
            float height = std::max(mhead->getSize().height, chatBack->getSize().height)+marginHeight;
            //pan->setSize(CCSizeMake(fs.width, height));
            //pan->setSizeType(SIZE_ABSOLUTE);
            //pan->setVisible(true);
            
            //使用customSize 而不是图片自动size
            img->ignoreContentAdaptWithSize(false);
            
            
            pan->setEnabled(true);
            pan->setSize(CCSizeMake(fs.width, height));
            pan->setSizeType(SIZE_ABSOLUTE);
            pan->setVisible(true);
            
            lv->pushBackCustomItem(pan);
            
            //发送图像文件
            
            string str;
            //0 text 1 voice 2 image
            trySendMsg(str, Channel::IMAGE_TYPE, error);
            
        }
    }
}

void ChatView::updateMidSize(float diff) {
    
    if (needAdjust) {
        sizeTime += diff;
        float rate =std::min(1.0f, sizeTime/totalTime);
        float curHei = (endHeight-startHeight)*rate+startHeight;
        
        midView->setSize(CCSizeMake(totalWidth, curHei));
        if (rate >= 1) {
            needAdjust = false;
        }
    }
    
}

void ChatView::checkError() {
    for (int i=0; i < errorMsg->count();) {
        CCNode *error = (CCNode*)errorMsg->objectAtIndex(i);
        int msgId = error->getTag();
        int state = getMsgState(msgId);
        
        //int state = Logic::getInstance()->getMsgState(msgId);
        //CCLog("check Error %d %d", msgId, state);
        //成功
        if (state == 1) {
            error->setVisible(false);
            errorMsg->removeObjectAtIndex(0);
        }else if(state == 2) {
            error->setVisible(true);
            errorMsg->removeObjectAtIndex(0);
        //不存在
        }else if(state == 1) {
            error->setVisible(false);
            errorMsg->removeObjectAtIndex(0);
        //正在发送中 == 0
        }else {
            break;
        }
    }
}


void ChatView::update(float diff){
    updateMidSize(diff);
    updateRecordTime(diff);
    checkError();
    sleepTime -= diff;
    //下一帧才开始 scroll
    //需要等待 listView 调用了 visit sortAllChildren 之后 refreshView 成功 更新了界面 才可以 驱动 scrollToBottom 来执行 这时候 内部容器的size才会变化
    if (needScroll != 0 && !needAdjust) {
        needScroll++;
        if (needScroll == waitScrollFrame) {
            lv->scrollToBottom(0.1, false);
        }else if(needScroll > waitScrollFrame) {
            needScroll = 0;
        }
        
        /*
        scrollTime--;
        if (scrollTime < 0) {
            
        }
         */
    }
    
    passTime += diff;
    if (passTime > 1) {
        time_t td;
        time(&td);
        struct tm *tin = localtime(&td);
        int h = tin->tm_hour;
        int mn = tin->tm_min;
        char buf[128];
        sprintf(buf, "%.2d:%.2d", h, mn);
        timeNow->setText(buf);
        passTime = 0;
    }
    
    
    updateEnter(diff);
    sendImg();
    
    receiveMsg();
    
}

void ChatView::pinyinMove(){
    bottom->stopAllActions();
    bottom->runAction(CCMoveTo::create(0.2, ccp(0, ksize.height)));
    
    CCSize sz = bottom->getSize();
    CCLog("bottom Size %f %f", sz.width, sz.height);
    
    //CCPoint per = midView->getSizePercent();
    //CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
    //float keepH = fs.height- fs.height*oldSizePer.y;
    //float leftHeight = fs.height-ksize.height;
    //float realh = leftHeight-keepH;
    
    
    //去除键盘高度后 剩余高度
    //float rh = leftHeight*oldSizePer.y;
    //lv->get  获取所有项目的高度 如果 高度不够  显示项目才移动
    //CCLog("current real show Height %f %f %f", keepH, leftHeight, realh);
    
    
    //midView->setSizeType(SIZE_ABSOLUTE);
    //midView->setSize(CCSizeMake(fs.width, realh));
    //midView->runAction(CCFadeIn::create(0.5));
    
    //midView->stopAllActions();
    
    
    CCSize isz = lv->getInnerContainerSize();
    CCSize selfsz = lv->getSize();
    CCLog("inner size is %f %f %f %f", isz.width, isz.height, selfsz.width, selfsz.height);
    //640 278.4  640  278.4
    
    //640 710.4
    //640 278
    
    midView->stopAllActions();
    midView->runAction(CCMoveTo::create(0.2, ccp(0, sz.height+ksize.height)));
    midView->runAction(CCSequence::create(CCDelayTime::create(delaySizeTime), CCCallFunc::create(this, callfunc_selector(ChatView::adjustAbs)), NULL));
    
    
    //lv->scrollToBottom(0.1, false);
    needScroll = 1;
}


void ChatView::startAdjustSize(float eh) {
    CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
    needAdjust = true;
    startHeight = midView->getSize().height;
    totalWidth = fs.width;
    endHeight = eh;
    sizeTime = 0;
}



void ChatView::adjustAbs() {
    CCLog("adjust Abs");
    
    CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
    float keepH = fs.height- fs.height*oldSizePer.y;
    float leftHeight = fs.height-ksize.height;
    float realh = leftHeight-keepH;
    
    
    //去除键盘高度后 剩余高度
    //float rh = leftHeight*oldSizePer.y;
    //lv->get  获取所有项目的高度 如果 高度不够  显示项目才移动
    CCLog("current real show Height %f %f %f", keepH, leftHeight, realh);
    
    
    //totalWidth = fs.width;
    
    midView->setSizeType(SIZE_ABSOLUTE);
    startAdjustSize(realh);
    
    //midView->setSize(CCSizeMake(fs.width, realh));
}



void ChatView::onText(cocos2d::CCObject *obj, TextFiledEventType tt) {
    switch (tt) {
        case cocos2d::ui::TEXTFIELD_EVENT_ATTACH_WITH_IME:
        {
            //出现键盘之后用户就不能点击了
            
            tf->setTouchEnabled(false);
            CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
            float nw = 400*fs.width/640;
            
            
            tf->setSize(CCSizeMake(nw, fs.height));
            inputBorder->loadTexture("gui/img_Chat_Enter_Green.png");
            
            
            showKeyboard = true;
            CCLog("setSizeYet %d",setSizeYet);
            /*
            //CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
            //bottom->runAction(CCMoveTo::create(0.225, ccp(0, screenSize.height/2)));
            if (setSizeYet) {
                bottom->runAction(CCMoveTo::create(0.225, ccp(0, ksize.height)));
            }
             */
            //CCEGLView *pegl = CCDirector::sharedDirector()->getOpenGLView();
            //pegl->setIMEKeyboardState(true);
            
        }
            break;
        case cocos2d::ui::TEXTFIELD_EVENT_DETACH_WITH_IME:
        {
            CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
            float nw = 400*fs.width/640;
            //当文本高度 高于这个高度不要 改变 只有 当点击发送的时候 才调整高度
            tf->setSize(CCSizeMake(nw, 58));
            
            //关闭键盘
            CCLog("event detach with ime");
            bottom->stopAllActions();
            bottom->runAction(CCMoveTo::create(0.225, ccp(0, 0)));
            CCSize bsz = bottom->getSize();
            
            //CCPoint per = midView->getSizePercent();
            //根据类型调整尺寸
            //midView->setSizeType(SIZE_PERCENT);
            //midView->setSizePercent(oldSizePer);
            
            //midView->updateSizeAndPosition();
            //等待对方 窗口滑到底部再调整size
            //midView->stopAllActions();
            
            
            
            
            midView->stopAllActions();
            midView->runAction(CCMoveTo::create(0.2, ccp(0, bsz.height)));
            midView->runAction(CCSequence::create(CCDelayTime::create(delaySizeTime), CCCallFunc::create(this, callfunc_selector(ChatView::adjustSize)), NULL));
            
            //CCEGLView *pegl = CCDirector::sharedDirector()->getOpenGLView();
            //pegl->setIMEKeyboardState(false);
            //
            tf->setTouchEnabled(true);
            
            //lv->scrollToBottom(0.1, false);
            needScroll = 1;
            inputBorder->loadTexture("gui/img_Chat_Enter.png");
            
        }
            break;
        case cocos2d::ui::TEXTFIELD_EVENT_INSERT_TEXT:
        {
            adjustBut();
        }
            break;
        case cocos2d::ui::TEXTFIELD_EVENT_DELETE_BACKWARD:
        {
            adjustBut();
        }
            break;
        default:
            break;
    }
}

void ChatView::adjustSize() {
    CCLog("adjust Size ");
    //midView->setSizeType(SIZE_PERCENT);
    //midView->setSizePercent(oldSizePer);
    CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
    startAdjustSize(fs.height*oldSizePer.y);
}


void ChatView::adjustBut() {
    string con = tf->getStringValue();
    UICCTextField *rd = tf->getRenderer();
    CCSize inputSize = tf->getSize();
    CCLog("input size is %f %f", inputSize.width, inputSize.height);
    chatText->setString("");
    chatText->setDimensions(CCSizeMake(inputSize.width, 0));
    chatText->setString(con.c_str());
    CCSize lsz = chatText->getContentSize();
    
    //多行 和 单行 文本  位置不同
    if (lsz.height >= 70) {
        tf->setPosition(ccp(oldPos.x, oldPos.y-15));
    }else {
        tf->setPosition(ccp(oldPos.x, oldPos.y));
    }
    
    
    
    //调整高度
    if (con.length() > 0) {
        add->setEnabled(false);
        send->setEnabled(true);
    } else {
        add->setEnabled(true);
        send->setEnabled(false);
    }
}