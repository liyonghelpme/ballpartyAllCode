//
//  Channel.h
//  TestCpp
//
//  Created by dozengame on 14-5-21.
//
//

#ifndef __TestCpp__Channel__
#define __TestCpp__Channel__

#include <iostream>
#include <string.h>
#include "cocos2d.h"
#include "HttpModel.h"
//#include "Message.h"
#include <map>
#include <vector>
class Message;
class Match;




using namespace std;
class Channel : public CCObject {
public:
    enum MESSAGE_TYPE {
        TEXT_TYPE,
        VOICE_TYPE,
        IMAGE_TYPE,
        QUIT_TYPE,
    };
    
    
    int cid;
    string m_serHost;
    string m_serPort;
    int state;
    
    vector<Message*> allMessages;
    
    //频道消息缓存 的开始时间和结束时间
    long long startTime;
    long long endTime;
    
    Channel();
    //void getHistoryMessage();
    
    bool sendMessage(string content, int contentType);
    
    void onReceiveMessage();
    
    void setData(Match*);
    
    Message* refreshMessage();
    
    void listenChannel();
    
};


#endif /* defined(__TestCpp__Channel__) */
