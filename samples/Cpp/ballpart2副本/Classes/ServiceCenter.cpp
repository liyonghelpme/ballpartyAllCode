//
//  ServiceCenter.cpp
//  TestCpp
//
//  Created by dozengame on 14-5-21.
//
//

#include "ServiceCenter.h"
#include "MatchService.h"

//外网redis服务器
string serverRedis = "113.108.228.190";



//正式 测试
//string serverRedis = "172.17.0.145";


static ServiceCenter *s_ser = NULL;
ServiceCenter *ServiceCenter::getInstance(){
    if (s_ser == NULL) {
        s_ser = new ServiceCenter(); 
    }
    return s_ser;
}

//服务中心
ServiceCenter::ServiceCenter() {
    //正式服务器
    //m_serverAddress = "http://172.17.0.145:91/";
    
    
    
    //外网服务器
    m_serverAddress = "http://113.108.228.190:91/";
    
    
    
    //13678972729
    
    
    //测试服务器
    //m_serverAddress = "http://172.17.0.45:5000/";
    
    
    CCLog("set server Address %s", m_serverAddress.c_str());
    
    m_userService = new UserService(m_serverAddress);
    m_matchService = new MatchService(m_serverAddress);
    m_messageService = new MessageService(m_serverAddress);
}

ServiceBase *ServiceCenter::getService(enum SERVICE_TYPE type){
    switch (type) {
        case USER_SERVICE:
        {
            
            return m_userService;
        }
            break;
        case MATCH_SERVICE:
        {
            return m_matchService;
        }
            break;
        case MESSAGE_SERVICE:
        {
            return m_messageService;
        }
            break;
        default:
            break;
    }
    return NULL;
}
