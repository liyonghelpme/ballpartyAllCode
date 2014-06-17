//
//  ServiceCenter.h
//  TestCpp
//
//  Created by dozengame on 14-5-21.
//
//

#ifndef __TestCpp__ServiceCenter__
#define __TestCpp__ServiceCenter__

#include <iostream>
#include "cocos2d.h"
#include "UserService.h"
#include "Message.h"
#include "MatchService.h"
#include "MessageService.h"
extern string serverRedis;
extern string openU;

class ServiceCenter : public cocos2d::CCObject{
public:
    static ServiceCenter* getInstance();
    enum SERVICE_TYPE {
        USER_SERVICE,
        MATCH_SERVICE,
        MESSAGE_SERVICE,
    };
    ServiceBase *getService(enum SERVICE_TYPE type);
    
    //string m_serverRedis;
private:
    string m_urlAddress;
    
    
    
    ServiceCenter();
    UserService *m_userService;
    MatchService *m_matchService;
    MessageService *m_messageService;
    
    string m_serverAddress, m_redisAddress;
    HttpModel *m_httpModel;
    
};

#endif /* defined(__TestCpp__ServiceCenter__) */
