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
#include "HttpNetMassage.h"
#include "ServiceBase.h"
#include "User.h"


using namespace cocos2d;
using namespace extension;
using namespace std;

class UserService:public ServiceBase
{
public:
    //static UserService* shareUserService();
    void login(string _loginName,string _password,CCNode* pTagert,MyHttpResp selector);
    
    void refereeNum(string referee, CCNode* pTagert,MyHttpResp selector);
    
    //注册手机
    void phoneNum(string loginname, string referrer, CCNode* pTagert,MyHttpResp selector);
    
    //验证注册码
    void verify(string loginname, string referrer, string authcode, CCNode* pTagert,MyHttpResp selector);
    
    void registe(string loginname, string password, string nick, string gender, string area,
                        string like, CCNode* pTaget,MyHttpResp selector);
    
    void setPerfect(string _userid, string _nick, string _gender, string _area, string _like,
                    CCNode* pTagert,MyHttpResp selector);
    
    //解析服务器过来的数据信息
    NetRegist analyzeRegistRect(string s);
    //bool analyzeLoginRect(string s);
    
    NetLogin analyzeLoginRect(string s);
    bool analyzePerfectRect(string s);
    NetReferrer analyzeRefereeRect(string s);
    NetBase analyzeAuthCodeRect(string s);
    
    UserService(string url)
    :ServiceBase(url)
    {
        
    };
    ~UserService(){};
    
    User *getUser();
private:
    //网络线程
    
    User user;
};
#endif /* defined(__BallParty__UserService__) */
