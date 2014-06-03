//
//  WorldCup.cpp
//  TestCpp
//
//  Created by dozengame on 14-5-14.
//
//

#include "WorldCup.h"
#include "Logic.h"
#include "ChatView.h"
#include "ConfigView.h"
#include "FriendView.h"
#include <stdlib.h>
#include "writer.h"
#include "stringbuffer.h"
#include "MatchService.h"
#include "ServiceCenter.h"

//#include "Match.h"

using namespace extension;
using namespace ui;


static char *teamName[] = {
    "阿尔及利亚",
    "阿根廷",
    "澳大利亚",
    "巴西",
    "比利时",
    "波黑",
    "德国",
    "俄罗斯",
    "厄瓜多尔",
    "法国",
    "哥伦比亚",
    "哥斯达黎加",
    "韩国",
    "荷兰",
    "洪都拉斯",
    "加纳",
    "喀麦隆",
    "科特迪瓦",
    "克罗地亚",
    "美国",
    "墨西哥",
    "尼日利亚",
    "葡萄牙",
    "日本",
    "瑞士",
    "乌拉圭",
    "西班牙",
    "希腊",
    "伊朗",
    "意大利",
    "英格兰",
    "智利",
};

int getTeamId(string tn) {
    for (int i = 0 ; i < 32; i++) {
        string tc = teamName[i];
        if (tc.compare(tn) == 0) {
            return i+1;
        }
    }
    return -1;
}

static CCScene *worldScene;
CCScene *WorldCup::scene(){
    CCScene *scene = CCScene::create();
    worldScene = scene;
    
    // 'layer' is an autorelease object
    WorldCup *layer = WorldCup::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}
WorldCup::~WorldCup(){
    dict->release();
}

bool WorldCup::init(){
    if ( !CCLayer::init() )
    {
        return false;
    }
    lastUpdateTime = 0;
    waitBasicTime = 0;
    
    
    state = 0;
    showYet = false;
    inUpdateData = false;
    oldOrNew = 0;
    lastRefreshTime = 0;
    inRefresh = false;
    lastUpdateIndex = 0;
    dict = CCDictionary::create();
    dict->retain();
    
    
    
    CCSize size = CCDirector::sharedDirector()->getVisibleSize();
    CCLog("winSize %f %f", size.width, size.height);
    
    UILayer *lay = UILayer::create();
    addChild(lay);
    //lay->setPosition(ccp(size.width/2, size.height/2));
    //lay->setContentSize(size);
    //if (Logic::getInstance()->fetchInfoState == 0) {
    //登录必须重新获取用户信息
    Logic::getInstance()->fetchInfo();
    //}
    

    //w = GUIReader::shareReader()->widgetFromJsonFile("gui/ballUI_1.json");
    
    w = GUIReader::shareReader()->widgetFromJsonFile("gui/ballUI_1_0.json");
    
    
    lay->addWidget(w);
    CCSize rs = w->getSize();
    CCLog("rootSize %f %f", rs.width, rs.height);
    w->setSizeType(SIZE_ABSOLUTE);
    w->setSize(size);
    
    Button *conf = static_cast<Button*>(UIHelper::seekWidgetByName(w, "selfinfo"));
    conf->addTouchEventListener(this, toucheventselector(WorldCup::onConf));
    /*
    
    */
    /*
    Button *fri = static_cast<Button*>(UIHelper::seekWidgetByName(w, "Button_5"));
    fri->addTouchEventListener(this, toucheventselector(WorldCup::onFri));
    */
    
    Button *group = static_cast<Button*>(UIHelper::seekWidgetByName(w, "grounp"));
    group->addTouchEventListener(this, toucheventselector(WorldCup::onGroup));
    
    
    lv = static_cast<UIListView*>(UIHelper::seekWidgetByName(w, "listView"));
    lv->addEventListenerListView(this, listvieweventselector(WorldCup::onListview));
    lv->addEventListenerScrollView(this, scrollvieweventselector(WorldCup::onScroll));
    lv->setBoundParam(5.0f);
    
    
    UIPanel *pan = static_cast<UIPanel*>(UIHelper::seekWidgetByName(w, "dayPanel"));
    pan->setEnabled(false);
    
    pc = pan->clone();
    pc->setVisible(true);
    pc->setEnabled(true);
    pc->retain();
    
    dlab = static_cast<UILabel*>(UIHelper::seekWidgetByName(pc, "date"));
    
    UIPanel *cupPanel = static_cast<UIPanel*>(UIHelper::seekWidgetByName(w, "cupPanel"));
    cupPanel->setEnabled(false);
    
    finCompete = static_cast<Layout*>(UIHelper::seekWidgetByName(w, "cupPanel_0"));
    notStartCom = static_cast<Layout*>(UIHelper::seekWidgetByName(w, "cupPanel_1"));
    finCompete->setEnabled(false);
    finCompete->setVisible(true);
    notStartCom->setEnabled(false);
    notStartCom->setVisible(true);
    
    chatButton = static_cast<Button*>(UIHelper::seekWidgetByName(finCompete, "chatButton"));
    
    
    Layout *lp = static_cast<Layout*>(UIHelper::seekWidgetByName(w, "cupPanel_0"));
    lp->setEnabled(false);
    
    //整个Panel 大小一样的 按钮
    //Button *full = static_cast<Button*>(UIHelper::seekWidgetByName(cupPanel, "full"));
    //full->setVisible(false);
    
    
    cp = cupPanel->clone();
    cp->setVisible(true);
    cp->setEnabled(true);
    cp->retain();
    
    
    scheduleUpdate();
    
    return true;
    
}


void WorldCup::onGroup(cocos2d::CCObject *obj, TouchEventType tt) {
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
            if (curInScene == NULL) {
                CCScene *ps = CCTransitionSlideInR::create(transTime, ChatView::scene(FEEDBACK));
                //防止冲入 多个 item 选择 如果进入了一个 房间 则不要 再次进入这个房间
                curInScene = ps;
                CCDirector::sharedDirector()->pushScene(ps);
                
            }

            //CCDirector::sharedDirector()->pushScene(getTransScene(ChatView::scene(FEEDBACK)));
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


//当移动到 顶部或者 底部 暂停滑动 更新数据
void WorldCup::onScroll(cocos2d::CCObject *obj, ScrollviewEventType st){
    CCLog("scroll event %d", st);
    switch (st) {
        //取消该功能
        
        //放手后回到顶部开始
        case cocos2d::ui::SCROLLVIEW_EVENT_SCROLL_TO_TOP:
        {
            if (!inUpdateData) {
                inUpdateData = true;
                oldOrNew = 0;
                MatchService *match = (MatchService*)ServiceCenter::getInstance()->getService(ServiceCenter::MATCH_SERVICE);
                bool fs = match->fetchOldMatchInfo();
                
                //bool fs = Logic::getInstance()->fetchOldMatchInfo();
                if (!fs) {
                    inUpdateData = false;
                } else {
                    match->clearTestNum();
                    
                    //Logic::getInstance()->clearTestNum();
                }
            }
        }
            break;
        //放手后 回到底部开始
        case cocos2d::ui::SCROLLVIEW_EVENT_SCROLL_TO_BOTTOM:
        {
            if (!inUpdateData) {
                inUpdateData = true;
                oldOrNew = 1;
                MatchService *match = (MatchService*)ServiceCenter::getInstance()->getService(ServiceCenter::MATCH_SERVICE);
                bool fs = match->fetchNewMatchInfo();
                if (!fs) {
                    inUpdateData = false;
                }else {
                    match->clearTestNum();
                    
                    //Logic::getInstance()->clearTestNum();
                }
            }
        }
            break;
         
        default:
            break;
    }
}


void WorldCup::onListview(cocos2d::CCObject *obj, ListViewEventType lt) {
    switch (lt) {
        case cocos2d::ui::LISTVIEW_ONSELECTEDITEM_START:
        {
            //CCLog("item number %d %f %f", lv->getItems()->count(), lv->getSize().width, lv->getSize().height);
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


void WorldCup::onFri(cocos2d::CCObject *obj, TouchEventType tt){
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
            CCDirector::sharedDirector()->pushScene(FriendView::scene());
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
void WorldCup::onConf(cocos2d::CCObject *obj, TouchEventType tt){
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
            if (curInScene == NULL) {
                //CCScene *ps = CCTransitionSlideInR::create(transTime, ChatView::scene(mid));
                //防止冲入 多个 item 选择 如果进入了一个 房间 则不要 再次进入这个房间
                curInScene = getTransScene(ConfigView::scene());
                //CCDirector::sharedDirector()->pushScene(ps);
                CCDirector::sharedDirector()->pushScene(curInScene);
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
//返回频道用户数量
void WorldCup::onRefresh(bool isSuc, std::string s, void *p){
    CCLog("refresh online number %d %s", isSuc, s.c_str());
    
    inRefresh = false;
    if (isSuc) {
        rapidjson::Document d;
        d.Parse<0>(s.c_str());
        //int count = d["data"].Size();
        
        int count = d["data"].GetInt();
        
        //int count = d["count"].GetInt();
        //cid 对应的 Layout对象
        
        //设置 matchId 比赛id  对应的 在线人数 mid -> 对应的比赛
        Layout *ly = (Layout*)(dict->objectForKey(*(int*)p));
        free(p);
        
        Label *onlineNum = (Label*)(UIHelper::seekWidgetByName(ly, "online"));
        if(onlineNum != NULL) {
            char onum[128];
            if (count >= 999) {
                sprintf(onum, "999+");
            }else
                sprintf(onum, "%d", count);
            
            onlineNum->setText(onum);
        }
        CCLog("online people %d", count);
    }
}

//更新数据所对应的 item的条目是那一条呢？
void WorldCup::refreshOnlineNum(float diff){
    //return;
    
    if (!inRefresh) {
        lastRefreshTime = lastRefreshTime-diff;
        if (lastRefreshTime <= 0 ) {
            //inRefresh = true;
            
            HttpModel *hm = HttpModel::getInstance();
            std::map<string, string> postData;
            //rapidjson::Document *d = Logic::getInstance()->d;
            //vector<Match> &allMatch = Logic::getInstance()->allMatch;
            
            MatchService *match = (MatchService*)ServiceCenter::getInstance()->getService(ServiceCenter::MATCH_SERVICE);
            vector<Match*> &allMatch = match->getAllMatch();
            
            //初始化比赛信息结束 之后 才可以 刷新比赛的 在线人数信息
            if (allMatch.size() > 0) {
            
                if (lastUpdateIndex < allMatch.size()) {
                    inRefresh = true;
                    //请求的
                    Match *mat = allMatch[lastUpdateIndex++];
                    //正在比赛中 才更新人数
                    if (mat->state == 1) {
                        int mid = mat->mid;
                        //char buf[128];
                        //sprintf(buf, "%d", mid);
                        //postData["cid"] =  buf;
                        
                        
                        int *cid = (int*)malloc(sizeof(int));
                        *cid = mid;
                        
                        //hm->addRequest("getUserCount", "GET", postData, this, MYHTTP_SEL(WorldCup::onRefresh), cid);
                        
                        
                        char buf[128];
                        sprintf(buf, "match/%d/user/0", *cid);
                        hm->addRequest(buf, "GET", postData, this, MYHTTP_SEL(WorldCup::onRefresh), cid);
                    }
                    
                                   
                }else {
                    //超过长度 则 处于refresh 状态
                    //inRefresh = true;
                    lastUpdateIndex = 0;
                    //下次更新100s之后
                    lastRefreshTime = 100;
                }
            }
        }
    }
}

//bug 比赛都给插入到 屁股后面去了 不对
void WorldCup::initItem(Match &c, float *delay, int *date, int *ord) {
    CCLog("initItem Match %d", c.mid);
    
    char on[1024];
    
    string emp = " ";
    
    string DAY[] = {
        "周日", "周一", "周二", "周三", "周四", "周五", "周六",
    };
    time_t now;
    time(&now);
    
    long long start_time = c.start_time;
    start_time /= 1000;
    
    tm *timeinfo;
    timeinfo = localtime((time_t*)&(start_time));
    int wd = timeinfo->tm_wday;
    int day = timeinfo->tm_yday;
    int mon = timeinfo->tm_mon;
    int mday = timeinfo->tm_mday;
    int hour = timeinfo->tm_hour;
    int min = timeinfo->tm_min;
    //new Data push dayPanel
    //不同天
    CCLog("data day si %d %d", *date, day);
    char minfo[128];
    if (*date != day) {
        sprintf(minfo, "%.2d/%.2d", mon+1, mday);
        *date = day;
        string fd = DAY[wd]+emp+minfo;
        dlab->setText(fd);
        Widget *tm = pc->clone();
        tm->setTag(-1);
        //lv->pushBackCustomItem(tm);
        if (*ord == -1) {
            lv->pushBackCustomItem(tm);
        }else {
            lv->insertCustomItem(tm, *ord);
            (*ord)++;
        }
    }
    
    
    
    //添加比赛信息
    //sprintf(minfo, "%.2d:%.2d", hour, min);
    //string tit = c.cate_name;
    //tit += emp+c.title+emp+minfo;
    
    
    Layout *ly;
    
    //Label *state;
    Label *onl;
    long long end_time = c.end_time;
    end_time /= 1000;
    int curstate = 0;
    //比赛已经结束
    CCLog("now end_time start_time %llf %llf %llf", (double)now, (double)start_time, (double)end_time);
    
    if (now >= end_time) {
        ly = (Layout*)finCompete->clone();
        ly->setEnabled(true);
        
        curstate = 0;
        
        //state = 1 比赛正在进行
    } else if(now >= start_time) {
        ly = (Layout*)cp->clone();
        ly->setEnabled(true);
        curstate = 1;
    } else {
        ly = (Layout*)notStartCom->clone();
        ly->setEnabled(true);
        curstate = 2;
        double stime = start_time-now;
        //超过24小时不能进入聊天 多长时间检测一次
    }
    UILabel *title = static_cast<UILabel*>(UIHelper::seekWidgetByName(ly, "round"));
    UILabel *online = static_cast<UILabel*>(UIHelper::seekWidgetByName(ly, "online"));
    UILabel *host = static_cast<UILabel*>(UIHelper::seekWidgetByName(ly, "host"));
    UILabel *client = static_cast<UILabel*>(UIHelper::seekWidgetByName(ly, "client"));
    ImageView *hImg = static_cast<ImageView*>(UIHelper::seekWidgetByName(ly, "hostImg"));
    ImageView *gImg = static_cast<ImageView*>(UIHelper::seekWidgetByName(ly, "guestImg"));
    Label *state = static_cast<Label*>(UIHelper::seekWidgetByName(ly, "state"));
    Button *bnt = static_cast<Button*>(UIHelper::seekWidgetByName(ly, "chatButton"));
    CCLog("chat button init");
    Button *realBnt = static_cast<Button*>(UIHelper::seekWidgetByName(ly, "realBnt"));
    
    
    //屏幕高度和设计高度的比例 按照比例调整一下item的Y方向高度比例 固定Y高度
    CCSize vs = CCDirector::sharedDirector()->getVisibleSize();
    float hc = 960/vs.height;
    CCPoint lsz = ly->getSizePercent();
    lsz.y = lsz.y*hc;
    //缩放高度Y
    ly->setSizePercent(lsz);
    
    
    char buf[512];
    int t1 = getTeamId(c.host_name);
    if (t1 != -1) {
        sprintf(buf, "flags/%d.png", t1);
        hImg->loadTexture(buf);
        adjustScale(hImg, 56, 56);
    }else {
        hImg->setVisible(false);
    }
    CCLog("getTeamId 1 over");
    
    int t2 = getTeamId(c.guest_name);
    if (t2 != -1) {
        sprintf(buf, "flags/%d.png", t2);
        gImg->loadTexture(buf);
        adjustScale(gImg, 56, 56);
    }else {
        gImg->setVisible(false);
    }
    
    host->setText(c.host_name);
    client->setText(c.guest_name);
    CCLog("getTeamId 2 over");
    
    
    bnt->getNormal()->runAction(CCSequence::create(CCDelayTime::create(*delay), CCFadeIn::create(0.2), NULL));
    *delay += 0.2;
    
    if (curstate == 0) {
        title->setText("完场");
        //结束后3小时内 仍可以进入
        //结束后3小时之外 不能进入 测试数据
        double finTime = now-end_time;
        CCLog("finish Time %lf", finTime);
        //允许任何时间进入 聊天室
        if (finTime >= 3600.0*3) {
            //realBnt->setEnabled(false);
            //bnt->setTouchEnabled(false);
        }
        
    }else if (curstate == 1) {
        string tit = c.cate_name;
        tit += emp+c.title;
        title->setText(tit);
        int ol = 0;
        sprintf(on, "%d", ol);
        online->setText(on);
        
        sprintf(minfo, "%.2d:%.2d", hour, min);
        state->setText(minfo);
    }else if(curstate == 2) {
        string tit = c.cate_name;
        tit += emp+c.title;
        title->setText(tit);
        
        sprintf(minfo, "%.2d:%.2d", hour, min);
        state->setText(minfo);
    }
    
    CCLog("chat button init");
    
    ly->setTag(c.mid);
    
    int cid = c.mid;
    
    realBnt->setTag(cid);
    realBnt->addTouchEventListener(this, toucheventselector(WorldCup::onChat));
    //比赛信息对应的 选择Item
    dict->setObject(ly, c.mid);
    
    //存储比赛当前的状态
    ly->setUserData((void*)&c);
    c.state = curstate;
    
    
    //插在尾部 还是 头部
    if (*ord == -1) {
        lv->pushBackCustomItem(ly);
    }else {
        lv->insertCustomItem(ly, *ord);
        (*ord)++;
    }
}


//将logic中得逻辑移动到 MatchService 里面去
//如果网络失败 则 重试获取 数据
void WorldCup::showBasic(){
    MatchService *match = (MatchService*)ServiceCenter::getInstance()->getService(ServiceCenter::MATCH_SERVICE);
    if (state == 0 && !match->getReqYet() && !inUpdateData) {
        state = 1;
        //异步
        match->getMatches(0, 0);
    }
    if (match->getInitYet()) {
        //重试发送消息
        if (match->getNetError()) {
            state = 0;
            CCLog("重试获取比赛信息");
            
        }else {
            showYet = true;
            
            vector<Match*> &allMatch = match->getAllMatch();
            float delay = 0;
            int date = -1;
            int ord = -1;
            lv->setTouchEnabled(false);
            for (int i=0; i < allMatch.size(); i++) {
                Match &c = (*allMatch[i]);
                initItem(c, &delay, &date, &ord);
            }
            lv->setTouchEnabled(true);
        }
    }
}


void WorldCup::showScroll() {
    MatchService *ms = (MatchService*)ServiceCenter::getInstance()->getService(ServiceCenter::MATCH_SERVICE);

    if (inUpdateData && ms->getInitYet()) {
        inUpdateData = false;
        //网络请求失败 则放弃
        if (ms->getNetError()) {
            return;
        }
        
        
        vector<Match*> &allMatch = ms->getAllMatch();
        time_t now;
        time(&now);
        float delay = 0;
        int date = -1;
        int ord = 0;
        //插在头部
        if (ms->startInd == 0) {
            ord = 0;
        //插在尾部
        }else {
            ord = -1;
        }
        lv->setTouchEnabled(false);
        for (rapidjson::SizeType i=ms->startInd; i < ms->endInd; i++) {
            Match &c = (*allMatch[i]);
            initItem(c, &delay, &date, &ord);
        }
        lv->setTouchEnabled(true);
    }
}

void WorldCup::updateCompetitionTime(float dt) {
    lastUpdateTime += dt;
    //10s 更新一次
    if (lastUpdateTime >= 10) {
        lastUpdateTime = 0;
        
        CCArray *matches = lv->getItems();
        
        //MatchService *match = (MatchService*)ServiceCenter::getInstance()->getService(ServiceCenter::MATCH_SERVICE);
        //vector<Match*> &allMatch = match->getAllMatch();
        for (int i = 0; i < matches->count(); i++) {
            Layout *ly = (Layout*)matches->objectAtIndex(i);
            int tag = ly->getTag();
            if (tag != -1) {
                CCLog("match id %d", tag);
                
                Match *c = (Match*)ly->getUserData();
                //重新计算比赛状态
                int curState = c->recaculateState();
                //替换旧的item 展示新的item
                if (curState != c->state) {
                    refreshMatchState(ly, i);
                }
                
            }
        }
    }
}

//刷新比赛状态  比赛的 是否完成 以及 当前时间 chatButton 高度

//测试方法： 每次插入不同的比赛
void WorldCup::refreshMatchState(cocos2d::ui::Layout *oldly, int itemId) {
    CCLog("refreshMatchState %d %d", oldly->getTag(), itemId);
    char on[1024];
    
    string emp = " ";
    
    string DAY[] = {
        "周日", "周一", "周二", "周三", "周四", "周五", "周六",
    };
    time_t now;
    time(&now);
    
    Match *c = (Match*)oldly->getUserData();
    int curstate = c->recaculateState();
    c->state = curstate;
    
    
    long long start_time = c->start_time;
    start_time /= 1000;
    
    tm *timeinfo;
    timeinfo = localtime((time_t*)&(start_time));
    int wd = timeinfo->tm_wday;
    int day = timeinfo->tm_yday;
    int mon = timeinfo->tm_mon;
    int mday = timeinfo->tm_mday;
    int hour = timeinfo->tm_hour;
    int min = timeinfo->tm_min;
    //new Data push dayPanel
    //不同天
    
    Layout *ly;
    
    //Label *state;
    Label *onl;
    long long end_time = c->end_time;
    end_time /= 1000;
    //比赛已经结束
    CCLog("refresh match state %llf %llf %llf", (double)now, (double)start_time, (double)end_time);
    if (curstate == 0) {
        ly = (Layout*)finCompete->clone();
    } else if(curstate == 1) {
        ly = (Layout*)cp->clone();
    } else {
        ly = (Layout*)notStartCom->clone();
    }
    ly->setEnabled(true);
    
    
    UILabel *title = static_cast<UILabel*>(UIHelper::seekWidgetByName(ly, "round"));
    UILabel *online = static_cast<UILabel*>(UIHelper::seekWidgetByName(ly, "online"));
    UILabel *host = static_cast<UILabel*>(UIHelper::seekWidgetByName(ly, "host"));
    UILabel *client = static_cast<UILabel*>(UIHelper::seekWidgetByName(ly, "client"));
    ImageView *hImg = static_cast<ImageView*>(UIHelper::seekWidgetByName(ly, "hostImg"));
    ImageView *gImg = static_cast<ImageView*>(UIHelper::seekWidgetByName(ly, "guestImg"));
    Label *state = static_cast<Label*>(UIHelper::seekWidgetByName(ly, "state"));
    Button *bnt = static_cast<Button*>(UIHelper::seekWidgetByName(ly, "chatButton"));
    CCLog("chat button init");
    
    //屏幕高度和设计高度的比例 按照比例调整一下item的Y方向高度比例 固定Y高度
    CCSize vs = CCDirector::sharedDirector()->getVisibleSize();
    float hc = 960/vs.height;
    CCPoint lsz = ly->getSizePercent();
    lsz.y = lsz.y*hc;
    //缩放高度Y
    ly->setSizePercent(lsz);
    
    
    Button *realBnt = static_cast<Button*>(UIHelper::seekWidgetByName(ly, "realBnt"));
    
    char buf[512];
    sprintf(buf, "flags/%d.png", getTeamId(c->host_name));
    hImg->loadTexture(buf);
    adjustScale(hImg, 80, 80);
    
    sprintf(buf, "flags/%d.png", getTeamId(c->guest_name));
    gImg->loadTexture(buf);
    adjustScale(gImg, 80, 80);
    host->setText(c->host_name);
    client->setText(c->guest_name);
    
    bnt->getNormal()->runAction(CCFadeIn::create(0.2));
    
    
    char minfo[256];
    if (curstate == 0) {
        title->setText("完场");
    }else if (curstate == 1) {
        string tit = c->cate_name;
        tit += emp+c->title;
        title->setText(tit);
        int ol = 0;
        sprintf(on, "%d", ol);
        online->setText(on);
        
        sprintf(minfo, "%.2d:%.2d", hour, min);
        state->setText(minfo);
    }else if(curstate == 2) {
        string tit = c->cate_name;
        tit += emp+c->title;
        title->setText(tit);
        
        sprintf(minfo, "%.2d:%.2d", hour, min);
        state->setText(minfo);
    }
    
    CCLog("chat button init");
    ly->setTag(c->mid);
    int cid = c->mid;
    
    realBnt->setTag(cid);
    realBnt->addTouchEventListener(this, toucheventselector(WorldCup::onChat));
    //比赛信息对应的 选择Item
    //刷新比赛对应的 item 对象
    dict->setObject(ly, c->mid);
    //存储比赛当前的状态
    ly->setUserData((void*)&c);
    c->state = curstate;
    
    lv->insertCustomItem(ly, itemId);
    //旧的id变成 +1
    lv->removeItem(itemId+1);
}


//停止更新数据
void WorldCup::update(float diff){
    refreshOnlineNum(diff);
    updateCompetitionTime(diff);
    
    //为显示的时候 首先显示基本页面 数据
    if (!showYet) {
        waitBasicTime += diff;
        //等待2s 才进入
        if (waitBasicTime > 0.7) {
            showBasic();
        }
    } else {
        showScroll();
    }
    
    //暂时不用这个
    //正在等待网络更新比赛数据 向上 向下
    //网络数据已经获得了
    
    
}

void WorldCup::onChat(cocos2d::CCObject *obj, TouchEventType tt){
    CCLog("onChat Button");
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
            Button *bnt = static_cast<Button*>(obj);
            int mid = bnt->getTag();
            /*
            //rapidjson::Document *d = Logic::getInstance()->d;
            vector<Match> &allMatch = Logic::getInstance()->allMatch;
            
            //const rapidjson::Value &b = (*d)["data"];
            //const rapidjson::Value &c = b[mid];
            
            int matchID = 0;
            for (int i = 0; i < allMatch.size(); i++) {
                if (allMatch[i].mid == mid) {
                    matchID = i;
                    break;
                }
            }
            */
            
            //Logic::getInstance()->matchInfo = &allMatch[realid];
            //MatchService *ms = (MatchService*)ServiceCenter::getInstance()->getService(ServiceCenter::MATCH_SERVICE);
            //Match *mat = ms->getMatchById(mid);
            
            //防止重入
            //CCScene *rs = CCDirector::sharedDirector()->getRunningScene();
            if (curInScene == NULL) {
                CCScene *ps = CCTransitionSlideInR::create(transTime, ChatView::scene(mid));
                //防止冲入 多个 item 选择 如果进入了一个 房间 则不要 再次进入这个房间
                curInScene = ps;
                CCDirector::sharedDirector()->pushScene(ps);
            
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

