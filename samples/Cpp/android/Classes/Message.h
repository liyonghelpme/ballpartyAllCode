//
//  Message.h
//  TestCpp
//
//  Created by dozengame on 14-5-21.
//
//

#ifndef __TestCpp__Message__
#define __TestCpp__Message__

#include <iostream>
#include <string.h>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "HttpModel.h"
#include "Channel.h"

using namespace std;
using namespace cocos2d;
using namespace extension;
using namespace ui;

class Message{
public:
    int mid;
    int sender;
    string senderName;
    string mtype;
    string content;
    long long send_time;
    int flagId;
    int voiceTime;
    
    //TODO： 将Message 构造成json
    string Serialize();
    void UnSerialize(string &con,  string &type);
    
    void setData(const rapidjson::Value&);
};




#endif /* defined(__TestCpp__Message__) */
