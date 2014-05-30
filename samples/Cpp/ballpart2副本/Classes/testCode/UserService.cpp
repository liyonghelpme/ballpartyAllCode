//
//  用户服务类
//  BallParty
//
//  Created by dozengame on 14-5-13.
//
//

#include "UserService.h"

UserService* UserService::m_userService = NULL;
UserService* UserService::shareUserService()
{
	if(!m_userService)
	{
		m_userService = new UserService();
		CCEGLView* eglView = CCEGLView::sharedOpenGLView();
		CCSize visibleSize = eglView->getFrameSize();
	}
	return m_userService;
}
void UserService::login(string _loginName,string _password,CCNode* pTagert,MyHttpResp selector)
{
   
    HttpModel *hm = HttpModel::getInstance();
    std::map<std::string, std::string> postData;
    postData["loginname"]=_loginName;
    postData["password"]=_password;
    hm->addRequest(NET_LOGIN,"POST", postData, pTagert, selector, NULL);
}
void UserService::registe(string _loginName,string _password,string _passwordconfirm,string  _referencePhoneNumber,CCNode* pTaget,MyHttpResp selector)
{
    HttpModel *hm = HttpModel::getInstance();
    std::map<std::string, std::string> postData;
    postData["loginname"]=_loginName;
    postData["password"]=_password;
    postData["passwordconfirm"]=_passwordconfirm;
    postData["referrer"]=_referencePhoneNumber;
    //postData["phoneNumber"]=_phoneNumber;
    hm->addRequest(NET_REGIST,"POST", postData, pTaget, selector, NULL);

}

void UserService::setPerfect(string _userid, string _nick, string _gender, string _area, string _like, CCNode* pTagert,MyHttpResp selector){
    HttpModel *hm = HttpModel::getInstance();
    std::map<std::string, std::string> postData;
    postData["nick"]=_nick;
    postData["gender"]=_gender;
    postData["area"]=_area;
    postData["like"]=_like;
    
    string m_url = "user/" + _userid;
    //postData["phoneNumber"]=_phoneNumber;
    hm->addRequest(m_url,"PUT", postData, pTagert, selector, NULL);
}


NetRegist UserService::analyzeRegistRect(string s)
{
    NetRegist regData;
    rapidjson::Document d;
    d.Parse<0>(s.c_str());
    regData.status = d["state"].GetInt();
    const rapidjson::Value &a = d["data"];
    if(regData.status != 1){
        int i = 0;
        regData.msg = a[i].GetString();
        //regData.msg = d["data"].GetString();
    }else{
        regData.userid = a["id"].GetInt();
    }
    return regData;
}

NetLogin UserService::analyzeLoginRect(string s)
{
    NetLogin loginData;
    rapidjson::Document d;
    d.Parse<0>(s.c_str());

    loginData.status = d["state"].GetInt();

    const rapidjson::Value &a = d["data"];
    
    if(loginData.status != 0)
    {
        loginData.isPerfect = a["state"].GetInt();
        loginData.userid = a["id"].GetInt();
        if(loginData.isPerfect != 0){
            CCUserDefault::sharedUserDefault()->setIntegerForKey("avatar", a["like_team"].GetInt());
            CCUserDefault::sharedUserDefault()->flush();
        }

    }
    return loginData;
}

bool UserService::analyzePerfectRect(string s)
{
    rapidjson::Document d;
    d.Parse<0>(s.c_str());
    int re = d["state"].GetInt();
    
    if(re ==0)
    {
        return false;
    }else{
        return true;
    }
    return false;
}


