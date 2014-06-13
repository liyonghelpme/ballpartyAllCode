//
//  用户服务类
//  BallParty
//
//  Created by dozengame on 14-5-13.
//
//

#include "UserService.h"

User *UserService::getUser() {
    return &user;
}

//登陆
void UserService::login(string _loginName,string _password,CCNode* pTagert,MyHttpResp selector)
{
    std::map<std::string, std::string> postData;
    postData["loginname"]=_loginName;
    postData["password"]=_password;
    invoke(NET_LOGIN,"POST", postData, pTagert, selector, NULL);
}

//验证推荐人
void UserService::refereeNum(string referee, CCNode* pTagert,MyHttpResp selector){
    std::map<std::string, std::string> postData;
    //postData["referee"]=referee;
    string m_url = "user/checkreferrer/" + referee;
    invoke(m_url,"GET", postData, pTagert, selector, NULL);
}

//注册手机
void UserService::phoneNum(string loginname, string referrer, CCNode* pTagert,MyHttpResp selector){
    std::map<std::string, std::string> postData;
    postData["loginname"]=loginname;
    postData["referrer"]=referrer;
    string m_url = "user/getauthcode";
    invoke(m_url,"POST", postData, pTagert, selector, NULL);
}

//验证注册码
void UserService::verify(string loginname, string referrer, string authcode, CCNode* pTagert,MyHttpResp selector){
    std::map<std::string, std::string> postData;
    postData["loginname"]=loginname;
    postData["referrer"]=referrer;
    postData["authcode"]=authcode;
    string m_url = "user/checkauthcode";
    invoke(m_url,"POST", postData, pTagert, selector, NULL);
}

//注册
void UserService::registe(string loginname, string password, string nick, string gender, string area, string like, CCNode* pTaget,MyHttpResp selector)
{

    std::map<std::string, std::string> postData;
    postData["loginname"]=loginname;
    postData["password"]=password;
    postData["nick"]=nick;
    postData["gender"]=gender;
    postData["area"]=area;
    postData["like"]=like;
    //postData["phoneNumber"]=_phoneNumber;
    invoke(NET_REGIST,"POST", postData, pTaget, selector, NULL);

}

//完成注册
void UserService::setPerfect(string _userid, string _nick, string _gender, string _area, string _like, CCNode* pTagert,MyHttpResp selector){

    std::map<std::string, std::string> postData;
    postData["nick"]=_nick;
    postData["gender"]=_gender;
    postData["area"]=_area;
    postData["like"]=_like;

    string m_url = "user/" + _userid;
    //postData["phoneNumber"]=_phoneNumber;
    invoke(m_url,"PUT", postData, pTagert, selector, NULL);
}

/*** Http 回调 ***/
//完成资料
bool UserService::analyzePerfectRect(string s)
{
    rapidjson::Document d;
    d.Parse<0>(s.c_str());
    int re = d["state"].GetInt();
    if(re == 1){
        return true;
    }else{
        return false;
    }
}
//推荐人
NetReferrer UserService::analyzeRefereeRect(string s)
{
    NetReferrer mData;
    rapidjson::Document d;
    d.Parse<0>(s.c_str());
    const rapidjson::Value &a = d["data"];
    
    mData.status = d["state"].GetInt();
    if(mData.status != 1){
        //const rapidjson::Value &a = d["data"];
        mData.msg = a["txt"].GetString();
        //regData.msg = d["data"].GetString();
    }else{
        mData.referrerId = a["id"].GetInt();
    }
    return mData;
}

NetBase UserService::analyzeAuthCodeRect(string s)
{
    NetBase mData;
    rapidjson::Document d;
    d.Parse<0>(s.c_str());
    const rapidjson::Value &a = d["data"];
    
    mData.status = d["state"].GetInt();
    mData.msg = a["txt"].GetString();
    
    return mData;
}


//登录
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

NetRegist UserService::analyzeRegistRect(string s)
{
    NetRegist regData;
    rapidjson::Document d;
    d.Parse<0>(s.c_str());
    const rapidjson::Value &a = d["data"];
    
    regData.status = d["state"].GetInt();
    if(regData.status != 1){
        //const rapidjson::Value &a = d["data"];
        int i = 0;
        regData.msg = a["txt"].GetString();
        //regData.msg = d["data"].GetString();
    }else{
        regData.msg = a["txt"].GetString();
        regData.userid = a["uid"].GetInt();
    }
    return regData;
}
