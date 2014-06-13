//
//  ServiceBase.h
//  ballparty
//
//  Created by dozengame on 14-5-24.
//
//

#ifndef __ballparty__ServiceBase__
#define __ballparty__ServiceBase__

#include <iostream>
#include "cocos2d.h"
#include <string>

#include "HttpModel.h"

class HttpModel;

using namespace cocos2d;
using namespace std;

class ServiceBase : public CCObject{
private:
    string m_serverAddress;
    
    //m_redisAddress = "172.17.0.145";
    HttpModel *m_httpModel;
    
public:
    ServiceBase(string url);
    void invoke(string url, string method, map<string, string> &postData,
                CCObject *pTagert, MyHttpResp pSelector, void *param);
};

#endif /* defined(__ballparty__ServiceBase__) */
