//
//  Logic.cpp
//  TestCpp
//
//  Created by dozengame on 14-5-14.
//
//

#include "Logic.h"
#include <string>
#include <map>
#include "writer.h"
#include "stringbuffer.h"
#include "ServiceCenter.h"

bool DEBUG = true;
float transTime = 0.2;
float IMG_WIDTH = 200;
float IMG_HEIGHT = 200;
int FEEDBACK = 10000;

CCScene *curInScene = NULL;


long long delayTime = 100;

CCScene *getTransScene(CCScene *s)
{
    return CCTransitionSlideInR::create(transTime, s);
}

void adjustScale(CCNode *n, float w, float h) {
    CCSize sz = n->getContentSize();
    float sca = std::min(w/sz.width, h/sz.height);
    CCLog("scale is %f", sca, w, sz.width);
    n->setScale(sca);
}


static Logic *s_Logic = NULL;
Logic::Logic():
initMatchYet(false)
, requestYet(false)
, uid(1)
, vid(1)
, chatInfoYet(false)
, initChatYet(false)
, searchYet(false)
, matchInfo(NULL)
, testNum(0)
, inRequest(false)
, noOld(false)
, noNew(false)
, dayRange(30.0)
, fetchInfoState(0)
, gender(0)
, lookOther(false)
, initUserDataYet(false)
, msgId(0)
, inChatRoom(false)

{
    srand(time(NULL));
    uid = rand()%1000;
    //d = new rapidjson::Document();
    flagId = rand()%32+1;
}

int Logic::getImgId(){
    return imgId++;
}
int Logic::getVid(){
    return vid++;
}

void Logic::resetVid() {
    vid--;
}

/*
int Logic::getUID() {
    return uid;
}
*/
void Logic::setUID(int i){
    uid = i;
}
Logic *Logic::getInstance(){
    if (s_Logic == NULL) {
        s_Logic = new Logic();
    }
    return s_Logic;
}
bool Logic::realFetchNewMatch(){
    testNum++;
    initMatchYet = false;
    inRequest = true;
    
    
    HttpModel *hm = HttpModel::getInstance();
    std::map<string, string> postData;
    double aday, bday;
    aday = lastDay;
    bday = lastDay+24*3600*dayRange*1000;
    lastDay = bday;
    
    CCLog("fetch new match %llf %llf", aday, bday);
    char t1[512];
    sprintf(t1, "match/%lld/%lld", (long long)aday, (long long)bday);
    
    hm->addRequest(t1, "GET", postData, this, MYHTTP_SEL(Logic::initNewMatchOver), NULL);
    
    return true;
}
bool Logic::fetchNewMatchInfo() {
    if (inRequest) {
        return false;
    }
    return realFetchNewMatch();
    
}

bool Logic::realFetchOldMatch(){
    testNum++;
    initMatchYet = false;
    inRequest = true;
    HttpModel *hm = HttpModel::getInstance();
    std::map<string, string> postData;
    //实际的显示的时间范围
    //lastDay = firstDay;
    
    double aday, bday;
    aday = firstDay-24*3600*dayRange*1000;
    bday = firstDay;
    firstDay = aday;
    
    //firstDay = firstDay-24*3600*7*1000;
    
    char t1[512];
    sprintf(t1, "match/%lld/%lld", (long long)aday, (long long)bday);
    
    
    hm->addRequest(t1, "GET", postData, this, MYHTTP_SEL(Logic::initOldMatchOver), NULL);
    return true;
}

//没有在请求状态才能获取比赛信息
//如果连着7天都没有比赛则无法获取到比赛信息了
//获取旧的比赛信息
bool Logic::fetchOldMatchInfo(){
    if (inRequest) {
        return false;
    }
    return realFetchOldMatch();
}


void Logic::initNewMatchOver(bool isSuc, string s, void *param){
    if (isSuc) {
        rapidjson::Document d2;
        
        d2.Parse<0>(s.c_str());
        //没有获得最后一条数据
        //最多 7*4 = 140 天的数据
        CCLog("initNewMatchOver %d %d %s", d2["data"].Size(), testNum, s.c_str());
        
        int gsize = d2["data"].Size();
        if (gsize == 0 && testNum < 3) {
            //delete d2;
            realFetchNewMatch();
        } else {
            if (gsize == 0) {
                noNew = true;
            }
            
            initMatchYet = true;
            inRequest = false;
            
            startInd = allMatch.size();
            //startInd = (*d)["data"].Size();
            endInd = startInd+d2["data"].Size();
            
            //rapidjson::Document::AllocatorType &alloc = d->GetAllocator();
            
            //CCLog("push new Match %d", gsize);
            
            int size = d2["data"].Size();
            CCLog("old data %d", d2["data"].Size());
            //popBack 会导致整个数组 被销毁
            //将d2 拼接到 d 上面
            
            
            for (int i = 0; i < size; i++) {
                rapidjson::Value &v = d2["data"][i];
                Match mat;
                mat.start_time = v["start_time"].GetInt64();
                mat.end_time = v["end_time"].GetInt64();
                mat.mid = v["id"].GetInt();
                mat.cate_name = v["cate_name"].GetString();
                mat.title = v["title"].GetString();
                mat.host_name = v["host_name"].GetString();
                mat.guest_name = v["guest_name"].GetString();
                
                allMatch.push_back(mat);
                
                //rapidjson::Value tobj = v;
                
                //push Value  会分配内存空间
                /*
                rapidjson::StringBuffer strbuf;
                rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
                v.Accept(writer);
                CCLog("out strin %s", strbuf.GetString());
                */
                
                //(*d)["data"].PushBack(v, alloc);
                
            }
            //将d2 的数据移动出来 原始的设置成NULL 然后再释放原始对象
            
            
            //CCLog("new Size is %d", (*d)["data"].Size());
            CCLog("new Size is %lu", allMatch.size());
            //d = d2;
            //delete d2;
            /*
            rapidjson::StringBuffer strbuf;
            rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
            d->Accept(writer);
            CCLog("out strin %s", strbuf.GetString());
            */
        }
        
    }
}

void Logic::initOldMatchOver(bool isSuc, string s, void *param){
    if (isSuc) {
        
        rapidjson::Document d2;// = new rapidjson::Document();
        d2.Parse<0>(s.c_str());
        
        //没有获得最后一条数据
        //最多 7*4 = 140 天的数据
        CCLog("initOldMatchOver %d %d", d2["data"].Size(), testNum);
        if (d2["data"].Size() == 0 && testNum < 3) {
            //delete d2;
            realFetchOldMatch();
        } else {
            initMatchYet = true;
            inRequest = false;
            
            
            startInd = 0;
            endInd = d2["data"].Size();
            
            //rapidjson::Document::AllocatorType &alloc = d2->GetAllocator();
            CCLog("old data %lu", allMatch.size());
            int size = d2["data"].Size();
            
            rapidjson::Value &d = d2["data"];
            for (int i = 0; i < size; i++) {
                Match mat;
                mat.mid = d[i]["id"].GetInt();
                mat.start_time = d[i]["start_time"].GetInt64();
                mat.end_time = d[i]["start_time"].GetInt64();
                mat.cate_name = d[i]["cate_name"].GetString();
                mat.title = d[i]["title"].GetString();
                mat.host_name = d[i]["host_name"].GetString();
                mat.guest_name = d[i]["guest_name"].GetString();
                allMatch.insert(allMatch.begin()+i, mat);
                
                //rapidjson::Value &v = (*d)["data"][i];
                //(*d2)["data"].PushBack(v, alloc);
            }
            //popBack 会导致整个数组 被销毁
            /*
            while (size > 0) {
                rapidjson::Value &v = (*d)["data"].PopBack();
                (*d2)["data"].PushBack(v, alloc);
                size--;
            }
             */
            
            /*
            for (int i = 0; i < (*d)["data"].Size(); i++) {
                rapidjson::Value &v = (*d)["data"].PopBack();
                (*d2)["data"].PushBack(v, alloc);
            }
             */
            //delete d;
            //d = d2;
        }
    }
}








void Logic::initMatchInfo(){
    requestYet = true;
    testNum++;
    HttpModel *hm = HttpModel::getInstance();
    std::map<string, string> postData;
    
    //postData["starttime"] = "0";
    //postData["endtime"] = "99999999";
    double today, tomorrow;
    
    if (!searchYet) {
        searchYet = true;
        //struct timeval tv;
        //gettimeofday(&tv, NULL);
        //today = tv.tv_sec+(tv.tv_usec/1000000.0);
        time_t td;
        time(&td);
        
        //假设 time_t 对应的是秒
        tm *timeinfo;
        timeinfo = localtime(&(td));
        
        //修改为本天开始的时间
        timeinfo->tm_min = 0;
        timeinfo->tm_sec = 0;
        timeinfo->tm_hour = 0;
        
        today = mktime(timeinfo);
        
        CCLog("today time %ld %lf", td, today);
        
        tomorrow = today+24*3600*dayRange;
        today *= 1000;
        tomorrow *= 1000;
        firstDay = today;
        lastDay = tomorrow;
    } else {
        //向后推迟7天
        //firstDay = lastDay;
        today = lastDay;
        tomorrow = lastDay+24*3600*dayRange*1000;
        lastDay = tomorrow;
        
        //lastDay += 24*3600*7*1000;
        
        //today = firstDay;
        //tomorrow = lastDay;
    }
    
    //从今天开始的比赛
    char t1[512];
    sprintf(t1, "match/%lld/%lld", (long long)today, (long long)tomorrow);
    
    hm->addRequest(t1, "GET", postData, this, MYHTTP_SEL(Logic::initMatchOver), NULL);
}

void Logic::initChatInfo() {
    
    
    chatInfoYet = true;
    
    HttpModel *hm = HttpModel::getInstance();
    std::map<string, string> postData;
    int cid;
    //if (DEBUG) {
    //    cid = 0;
    //}else {
    MessageService *ms = (MessageService*)ServiceCenter::getInstance()->getService(ServiceCenter::MESSAGE_SERVICE);
    Channel *ch = ms->getCurChannel();
    
    cid = ch->cid;
    //}
    char buf[128];
    //sprintf(v, "%d", cid);
    //postData["cid"] = v;
    
    CCLog("init Chat Info %d", cid);
    
    sprintf(buf, "match/%d/user/1", cid);
    //hm->addRequest("getChatInfo", "GET", postData, this, MYHTTP_SEL(Logic::initChatOver), NULL);
    hm->addRequest(buf, "GET", postData, this, MYHTTP_SEL(Logic::initChatOver), NULL);
}

void Logic::initChatOver(bool isSuc, string s, void *param){
    CCLog("init chat information over %s", s.c_str());
    initChatYet = true;
    if (isSuc) {
        chatD.Parse<0>(s.c_str());
    } else {
        //放弃本次网络请求 的 聊天室数据 重新开始 请求
        initChatYet = false;
        chatInfoYet = false;
    }
}

void Logic::initMatchOver(bool isSuc, string s, void *param) {
    //initMatchYet = true;
    rapidjson::Document p;
    p.Parse<0>(s.c_str());
    
    //没有获得最后一条数据
    //最多 7*20 = 140 天的数据
    CCLog("initMatchOver %d", p["data"].Size());
    if (p["data"].Size() == 0 && testNum < 20) {
        this->initMatchInfo();
    } else {
        initMatchYet = true;
        rapidjson::Value &d = p["data"];
        for (int i=0; i < d.Size(); i++) {
            Match mat;
            mat.mid = d[i]["id"].GetInt();
            mat.start_time = d[i]["start_time"].GetInt64();
            mat.end_time = d[i]["start_time"].GetInt64();
            mat.cate_name = d[i]["cate_name"].GetString();
            mat.title = d[i]["title"].GetString();
            mat.host_name = d[i]["host_name"].GetString();
            mat.guest_name = d[i]["guest_name"].GetString();
            
            allMatch.push_back(mat);
        }
    }
    
    /*
    if (d["state"].GetInt() == 0) {
        return;
    }
    */
    
    
    /*
    const rapidjson::Value&b = d["matches"];
    for(rapidjson::SizeType i=0; i < b.Size(); i++) {
        const rapidjson::Value &c = b[i];
        
    }
    */
}
void Logic::storeData() {
    CCUserDefault *u = CCUserDefault::sharedUserDefault();
    u->setStringForKey("loginName", loginName);
    u->setStringForKey("nickname", nickname);
    u->setIntegerForKey("uid", uid);
}
/*
//比赛本身的id 作为频道ID
int Logic::getCID() {
    if (matchInfo != NULL) {
        return matchInfo->mid;
    }
    return 0;
}
*/

int Logic::getSenderId(){
    return uid;
    
    //char buf[256];
    //sprintf(buf, "%d", uid);
    //return string(buf);
}

void Logic::fetchInfo() {
    CCLog("fetchInfo");
    fetchInfoState = 0;
    if (fetchInfoState == 0) {
        fetchInfoState = 1;
        HttpModel *hm = HttpModel::getInstance();
        char buf[256];
        UserService *us  = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
        User *user = us->getUser();
        if (!lookOther) {
            sprintf(buf, "user/%d", user->uid);
        }else {
            sprintf(buf, "user/%d", otherId);
        }
        
        std::map<string, string> postData;
        hm->addRequest(buf, "GET", postData, this, MYHTTP_SEL(Logic::fetchOver), NULL);
    }
}

/*
 {"state":1,"data":{"id":1,"realName":"wang","phoneNumber":"13678972729","bio":"13678972729","avatar":"abc","confirmTime":null,"registerTime":null,"gender":1,"likeTeam":5,"area":"ccc","state":1}}
 
 {"state":1,"data":{"id":34,"realName":"%ffffffe6%ffffff9c%ffffff9f%ffffffe5%ffffffbe%ffffff85","phoneNumber":"13128513873","bio":null,"avatar":null,"confirmTime":null,"registerTime":1400839190513,"gender":1,"likeTeam":7,"area":"%ffffffe6%ffffff98%ffffffaf%ffffffe4%ffffffb8%ffffff8d%ffffffe6%ffffff98%ffffffaf","state":1}}
 */
void Logic::fetchOver(bool isSuc, string s, void *param) {
    fetchInfoState = 2;
    if (isSuc) {
        CCLog("fetchOver info %s", s.c_str());
        
        rapidjson::Document d;
        d.Parse<0>(s.c_str());
        const rapidjson::Value &bd = d["data"];
        if (d["state"].GetInt() == 0) {
            fetchInfoState = 0;
            return;
        }
        
        if (lookOther) {
            otherUser.setData(bd);
            lookOther = false;
            
        }else {
            UserService *us  = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
            User *user = us->getUser();
            user->setData(bd);
            initUserDataYet = true;
            
        }
        
        /*
        realName = bd["realName"].GetString();
        phoneNumber = bd["phoneNumber"].GetString();
        if(!bd["bio"].IsNull()) {
            bio = bd["bio"].GetString();
        }
        gender = bd["gender"].GetInt();
        flagId = bd["likeTeam"].GetInt();
        area = bd["area"].GetString();
        referrer = bd["referrer"].GetString();
        CCLog(referrer.c_str());
        */
        
        
        
        
        //UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
        //User *user = us->getUser();
        
    }else {
        fetchInfoState = 0;
    }
}


void Logic::clearState() {
    initUserDataYet = false;
    curInScene = NULL;
    fetchInfoState = 0;
}



