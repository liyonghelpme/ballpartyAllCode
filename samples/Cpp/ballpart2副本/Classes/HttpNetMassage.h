//
//  HttpNetMassage.h
//  BallParty
//  网络传输配置类
//  Created by dozengame on 14-5-14.
//
//

#ifndef BallParty_HttpNetMassage_h
#define BallParty_HttpNetMassage_h

#include "cocos2d.h"
using namespace std;
#define NET_HOST "http://172.17.0.145:91/"//ip地址
#define NET_LOGIN "user/login" //登陆
#define NET_REGIST "user/register"//注册
//#define NET_REFEREE "user/checkerferrer" //推荐人
//#define NET_SETPERFECT "user/setprefect"    //修改资料



//消息基本数据结构..供底层使用
//通用
typedef struct NetBase
{
    int status;
    string msg;
}NetBase;

typedef struct NetReferrer
{
    int status;
    string msg;
    int referrerId;
}NetReferrer;

//登陆
typedef struct NetLogin
{
    int status;
    int userid;
    int avatar;
    int isPerfect;  //是否完善资料
}NetLogin;

//注册
typedef struct NetRegist
{
    int status;
    int userid;
    string msg;
    
}NetRegist;

#endif
