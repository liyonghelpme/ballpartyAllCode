//
//  MessageService.cpp
//  ballparty
//
//  Created by dozengame on 14-5-24.
//
//

#include "MessageService.h"
#include "MatchService.h"
#include "ServiceCenter.h"
#include "Logic.h"

//, long long startTime, long long endTime
//nodejs 开启监控


//1401332420736  服务器时间
//1401332411000  本地时间比服务器 晚了 10s 时间


//1401332641000 客户端时间
//1401332645666 服务器时间  4 s 的时间 和 服务器上面相差
void MessageService::getHistoryMessage(int cid, CCObject *obj, MyHttpResp rp, bool initReq){
    CCLog("getHistoryMessage 获取服务器历史消息 如何测试？");
    
    char buf[512];
    long long startTime, endTime;
    
    //当前时间
    time_t td;
    time(&td);
    //tm *timeinfo;
    //timeinfo = localtime(&(td));
    if (initReq) {
        endTime = td+delayTime;//当期时间向后延续一些
        startTime = endTime-60*5; //向前5分钟
        
        startTime *= 1000;
        endTime *= 1000;
        
        hisStartTime = startTime;
        hisEndTime = endTime;
    } else {
        endTime = hisStartTime;
        startTime = hisStartTime-60*5;
        hisStartTime = startTime;
        hisEndTime = endTime;
    }
    
    sprintf(buf, "message/%d/%lld/%lld", cid, startTime, endTime);
    std::map<string, string> postData;
    
    
    invoke(buf, "GET", postData, obj, rp, NULL);
}


//向下移动回去 5分钟
void MessageService::restoreHisTime() {
    hisStartTime = hisEndTime;
    hisEndTime += 60*5;
}




Channel *MessageService::openChannel(const int matchID){
    CCLog("openChannel %d", matchID);
    
    MatchService *matchService = (MatchService*)ServiceCenter::getInstance()->getService(ServiceCenter::MATCH_SERVICE);
    
    //TODO @liyong match 构造 频道
    Match *match = matchService->getMatchById(matchID);
    
    if (match == NULL) {
        m_channel.cid = matchID;
    } else {
    //m_channel = new Channel();
        m_channel.setData(match);
    }
    return &m_channel;
}


