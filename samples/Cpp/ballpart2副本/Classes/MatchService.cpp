//
//  MatchService.cpp
//  ballparty
//
//  Created by dozengame on 14-5-24.
//
//

#include "MatchService.h"
#include "Match.h"
#include "Logic.h"

MatchService::MatchService(string url)
: m_requestYet(false)
, m_testNum(0)
, m_searchYet(false)
, m_dayRange(5.0)
, m_initMatchYet(false)
, ServiceBase(url)
, noOld(false)
, noNew(false)
, netError(false)
, updateTime(0)
{
   // d = new rapidjson::Document();
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(MatchService::onUpdate), this, 1, false);
}
//1分钟冷却一下 这样可以 获取 新添加的数据
void MatchService::onUpdate(float dt) {
    if (noNew || noOld) {
        updateTime += dt;
        if (updateTime >= 10) {
            updateTime = 0;
            noNew = false;
            noOld = false;
        }
    }
}

bool MatchService::getMatches(const double startTime, const double endTime) {
    CCLog("getMatches");
    if (m_requestYet) {
        return false;
    }
    //每次初始化比赛信息 都 初始化时间 退出登录 再进入 worldCup 页面
    m_searchYet = false;
    realRequestMatch();
}



/*
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
*/



void MatchService::realRequestMatch(){
    CCLog("real Request Match");
    //开始请求 未获得结果数据
    m_requestYet = true;
    m_initMatchYet = false;
    
    m_testNum++;
    std::map<string, string> postData;
    double today, tomorrow;
    
    if (!m_searchYet) {
        m_searchYet = true;
        time_t td;
        time(&td);
        
        //假设 time_t 对应的是秒
        tm *timeinfo = localtime(&(td));
        //修改为本天开始的时间
        timeinfo->tm_min = 0;
        timeinfo->tm_sec = 0;
        timeinfo->tm_hour = 0;
        
        today = mktime(timeinfo);
        
        CCLog("today time %ld %lf", td, today);
        
        tomorrow = today+24*3600*m_dayRange;
        today *= 1000;
        tomorrow *= 1000;
        m_firstDay = today;
        m_lastDay = tomorrow;
    } else {
        //向后推迟7天
        //firstDay = lastDay;
        today = m_lastDay;
        tomorrow = m_lastDay+24*3600*m_dayRange*1000;
        m_lastDay = tomorrow;
        
    }
    
    //从今天开始的比赛
    char t1[512] = {};
    sprintf(t1, "match/%lld/%lld", (long long)today, (long long)tomorrow);
    
    invoke(t1, "GET", postData, this, MYHTTP_SEL(MatchService::initMatchOver), NULL);
}


/*
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
*/


void MatchService::initMatchOver(bool isSuc, string s, void *param){
    if (isSuc) {
        netError = false;
        rapidjson::Document json;
        json.Parse<0>(s.c_str());
        //没有获得最后一条数据
        //最多 7*20 = 140 天的数据
        //CCLog("initMatchOver %d", (*d)["data"].Size());
        CCLog("initMatch Over %d", json["data"].Size());
        if (json["data"].Size() == 0 && m_testNum < 20) {
            this->realRequestMatch();
        } else {
            CCLog("finish initMatch");
            m_requestYet = false;
            m_initMatchYet = true;
            for (int i=0; i < json["data"].Size(); i++) {
                
                Match *match = new Match();
                match->setData(json["data"][i]);
                m_matchList.push_back(match);
                
            }
        }
    }else {
        m_requestYet = false;
        m_initMatchYet = true;
        netError = true;
    }
}



static Match feedbackMatch;

Match *MatchService::getMatchById(int mid) {
    Match *match = NULL;
    if (mid == FEEDBACK) {
        /*
        int mid;
        string cate_name;
        string title;
        string host_name;
        string guest_name;
        long long start_time;
        long long end_time;
        */
        feedbackMatch.mid = mid;
        feedbackMatch.cate_name = "用户反馈";
        feedbackMatch.title = "";
        feedbackMatch.host_name = "";
        feedbackMatch.guest_name = "";
        feedbackMatch.start_time = 0;
        feedbackMatch.end_time = 0;
        return &feedbackMatch;
    }
    
    for (int i = 0; i < m_matchList.size(); i++) {
        match = m_matchList[i];
        if (match->mid == mid) {
            return match;
        }
    }
    
    return NULL;
}


bool MatchService::fetchOldMatchInfo(){
    CCLog("fetchOldMatchInfo");
    if (m_requestYet || noOld) {
        return false;
    }
    return realFetchOldMatch();
}


bool MatchService::realFetchOldMatch(){
    m_testNum++;
    m_initMatchYet = false;
    m_requestYet = true;
    HttpModel *hm = HttpModel::getInstance();
    std::map<string, string> postData;
    //实际的显示的时间范围
    //lastDay = firstDay;
    
    double aday, bday;
    aday = m_firstDay-24*3600*m_dayRange*1000;
    bday = m_firstDay;
    m_firstDay = aday;
    
    
    char t1[512];
    sprintf(t1, "match/%lld/%lld", (long long)aday, (long long)bday);
    
    
    invoke(t1, "GET", postData, this, MYHTTP_SEL(MatchService::initOldMatchOver), NULL);
    return true;
}

bool MatchService::fetchNewMatchInfo() {
    if (m_requestYet || noNew) {
        return false;
    }
    return realFetchNewMatch();
    
}
bool MatchService::realFetchNewMatch(){
    m_testNum++;
    m_initMatchYet = false;
    m_requestYet = true;
    
    
    //HttpModel *hm = HttpModel::getInstance();
    std::map<string, string> postData;
    double aday, bday;
    aday = m_lastDay;
    bday = m_lastDay+24*3600*m_dayRange*1000;
    m_lastDay = bday;
    
    CCLog("fetch new match %llf %llf", aday, bday);
    char t1[512];
    sprintf(t1, "match/%lld/%lld", (long long)aday, (long long)bday);
    
    invoke(t1, "GET", postData, this, MYHTTP_SEL(MatchService::initNewMatchOver), NULL);
    
    return true;
}

void MatchService::initNewMatchOver(bool isSuc, string s, void *param){
    if (isSuc) {
        rapidjson::Document d2;
        
        d2.Parse<0>(s.c_str());
        //没有获得最后一条数据
        //最多 7*4 = 140 天的数据
        CCLog("initNewMatchOver %d %d %s", d2["data"].Size(), m_testNum, s.c_str());
        
        int gsize = d2["data"].Size();
        //if (gsize == 0 && m_testNum < 3) {
            //delete d2;
        //    realFetchNewMatch();
        //} else {
            if (gsize == 0) {
                noNew = true;
                m_lastDay -= 24*3600*m_dayRange*100;
            }
            
            m_initMatchYet = true;
            m_requestYet = false;
            
            startInd = m_matchList.size();
            //startInd = (*d)["data"].Size();
            endInd = startInd+d2["data"].Size();
            
            //CCLog("push new Match %d", gsize);
            
            int size = d2["data"].Size();
            CCLog("old data %d", d2["data"].Size());
            //popBack 会导致整个数组 被销毁
            //将d2 拼接到 d 上面
            
            
            for (int i = 0; i < size; i++) {
                rapidjson::Value &v = d2["data"][i];
                Match *mat = new Match();
                mat->setData(v);
                m_matchList.push_back(mat);
            
            }
            //将d2 的数据移动出来 原始的设置成NULL 然后再释放原始对象
            //CCLog("new Size is %lu", allMatch.size());
        //}
        netError = false;
        
    } else {
        m_initMatchYet = true;
        m_requestYet = false;
        netError = true;
        
        m_lastDay -= 24*3600*m_dayRange*1000;
    }
}




void MatchService::initOldMatchOver(bool isSuc, string s, void *param) {
    if (isSuc) {
        rapidjson::Document d2;// = new rapidjson::Document();
        d2.Parse<0>(s.c_str());
        //没有获得最后一条数据
        //最多 7*4 = 140 天的数据
        CCLog("initOldMatchOver %d %d", d2["data"].Size(), m_testNum);
        //if (d2["data"].Size() == 0 && m_testNum < 3) {
            //delete d2;
        //    realFetchOldMatch();
        //} else {
            if (d2["data"].Size() == 0) {
                noOld = true;
                //获取失败恢复时间
                m_firstDay += 24*3600*m_dayRange*1000;
            }
        
            
            m_initMatchYet = true;
            m_requestYet = false;
            
            startInd = 0;
            endInd = d2["data"].Size();
            
            //rapidjson::Document::AllocatorType &alloc = d2->GetAllocator();
            CCLog("old data %lu", m_matchList.size());
            int size = d2["data"].Size();
            
            rapidjson::Value &d = d2["data"];
            for (int i = 0; i < size; i++) {
                Match *mat = new Match();
                mat->setData(d[i]);
                m_matchList.insert(m_matchList.begin()+i, mat);
                
            }

        //}
    } else {
        m_initMatchYet = true;
        m_requestYet = false;
        netError = true;
        m_firstDay += 24*3600*m_dayRange*1000;
    }
}
