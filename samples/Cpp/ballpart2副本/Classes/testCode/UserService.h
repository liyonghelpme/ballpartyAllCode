//
//  用户业务类
//  BallParty
//
//  Created by dozengame on 14-5-13.
//
//

#ifndef __BallParty__UserService__
#define __BallParty__UserService__

#include "cocos2d.h"
#include "User.h"
#include "curl/curl.h"
#include "cocos-ext.h"
#include "HttpModel.h"
using namespace cocos2d;
using namespace extension;
using namespace std;

class UserService:public CCObject
{
public:
    static UserService* shareUserService();
    void login(string _loginName,string _password,CCNode* pTagert,MyHttpResp selector);
    void registe(string _loginName,string _password,string _passwordconfirm,string _referencePhoneNumber,CCNode* pTaget,MyHttpResp selector);
    void setPerfect(string _userid, string _nick, string _gender, string _area, string _like, CCNode* pTagert,MyHttpResp selector);
    //解析服务器过来的数据信息
    NetRegist analyzeRegistRect(string s);
    NetLogin analyzeLoginRect(string s);
    bool analyzePerfectRect(string s);
    UserService(){};
    ~UserService(){};
private:
    //网络线程
    //pthread_t pDataTread;
    static UserService* m_userService;
    
    
};
#endif /* defined(__BallParty__UserService__) */
