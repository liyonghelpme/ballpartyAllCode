//
//  User.cpp
//  TestCpp
//
//  Created by dozengame on 14-5-21.
//
//

#include "User.h"
#include "HttpModel.h"
#include "Md5.h"

User::User()
:initDataYet(false)
{
    
}

void User::setChatInfo(const rapidjson::Value &bd) {
    if (bd["real_name"].IsString()) {
        realName = bd["real_name"].GetString();
        loginName = bd["real_name"].GetString();
    }else {
        realName = "";
        loginName = "";
    }
    
    
    //phoneNumber = bd["phone_number"].GetString();
    if(!bd["bio"].IsNull()) {
        bio = bd["bio"].GetString();
    }
    if (bd["gender"].IsInt()) {
        gender = bd["gender"].GetInt();
    }
    if (bd["like_team"].IsInt()) {
        flagId = bd["like_team"].GetInt();
    } else
        flagId = 1;
    
    //area = bd["area"].GetString();
    //referrer = bd["referrer"].GetString();
}

void User::setData(const rapidjson::Value &bd) {
    realName = bd["realName"].GetString();
    loginName = bd["realName"].GetString();
    
    phoneNumber = bd["phoneNumber"].GetString();
    if(!bd["bio"].IsNull()) {
        bio = bd["bio"].GetString();
    }
    gender = bd["gender"].GetInt();
    flagId = bd["likeTeam"].GetInt();
    area = bd["area"].GetString();
    referrer = bd["referrer"].GetString();
    referrer_nick = referrer;
    
    /*
    if (bd["referrer_nick"].IsString()) {
        referrer_nick = bd["referrer_nick"].GetString();
    }
     */
    
}
/*
static UserService *s_su = NULL;
UserService *UserService::getInstance() {
    if (s_su == NULL) {
        s_su = new UserService();
    }
    return s_su;
}
UserService::UserService()
{
    
}

void UserService::login(string loginName, string password, CCObject *obj, MyHttpResp rp){
    HttpModel *hm = HttpModel::getInstance();
    std::map<string, string>pd;
    pd["loginName"] = loginName;
    pd["password"] = md5(password);
    
    hm->addRequest("user/login", "POST", pd, obj, rp, NULL);
}




void UserService::registerUser(string loginName, string password, string realName, string phoneNum, string referencePhoneNumber, CCObject*obj, MyHttpResp rp){
    HttpModel *hm = HttpModel::getInstance();
    std::map<string, string>pd;
    pd["loginName"] = loginName;
    pd["password"] = md5(password);
    
    //pd["password"] =
    hm->addRequest("user/register", "POST", pd, obj, rp, NULL);
}

void UserService::saveProfile(){
    
}
void UserService::changePassword(){
    
}
void UserService::confirmReference(){
    
}
*/