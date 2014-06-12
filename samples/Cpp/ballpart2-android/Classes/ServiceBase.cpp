//
//  ServiceBase.cpp
//  ballparty
//
//  Created by dozengame on 14-5-24.
//
//

#include "ServiceBase.h"
#include "HttpModel.h"
#include "cocos2d.h"

ServiceBase::ServiceBase(string url) {
    m_serverAddress = url;
    
    //m_redisAddress = "172.17.0.145";
    
    m_httpModel = HttpModel::getInstance();
    m_httpModel->init(m_serverAddress);
    
    //baseRedisHost = "172.17.0.145";
}
void ServiceBase::invoke(string url, string method, map<string, string> &postData,
                         CCObject *pTagert, MyHttpResp pSelector, void *param) {
    m_httpModel->addRequest(url, method, postData, pTagert, pSelector, param);
}