//
//  MessageService.h
//  ballparty
//
//  Created by dozengame on 14-5-24.
//
//

#ifndef __ballparty__MessageService__
#define __ballparty__MessageService__

#include <iostream>
#include "ServiceBase.h"
#include "HttpModel.h"
#include "Channel.h"

class Channel;

class MessageService : public ServiceBase {
public:
    MessageService(string url)
    :ServiceBase(url){
        hisStartTime = 0;
        hisEndTime = 0;
    }
    //当前聊天的频道
    Channel *openChannel(int channelId);
    
    //获取服务器历史消息
    //, long long startTime, long long endTime
    void getHistoryMessage(int cid, CCObject *, MyHttpResp, bool initReq);
    
    void channelSendVoice(const char*fn);
    void channelSendText(const char*text);
    void channelSendImage();
    
    Channel *getCurChannel() {
        return &m_channel;
    }
    void restoreHisTime();
    
private:
    Channel m_channel;
    
    long long hisStartTime, hisEndTime;
    
};



#endif /* defined(__ballparty__MessageService__) */
