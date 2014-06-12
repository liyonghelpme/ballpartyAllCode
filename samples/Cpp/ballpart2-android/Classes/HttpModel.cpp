//
//  HttpModel.cpp
//  TestCpp
//
//  Created by dozengame on 14-5-13.
//
//

#include "HttpModel.h"
#include <cctype>
#include <iomanip>
#include <sstream>
#include "ServiceCenter.h"


using namespace std;

static HttpModel *s_http = NULL;

HttpModel *HttpModel::getInstance(){
    if (s_http == NULL) {
        s_http = new HttpModel();
    }
    return s_http;
}

void HttpModel::init(string url){
    baseUrl = url;
    firstTry = true;
    netError = false;
    errorTime = 0;
    tryTime = 10;
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(HttpModel::update), this, 1, false);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(HttpModel::onNetState), "netError", NULL);
    //baseRedisHost = redisurl;
}


void HttpModel::onNetState(cocos2d::CCObject *obj) {
    if (errorTime < 0) {
        //第一次试验网络状态 出错 则忽略本次错误 底层恢复一下状态
        //if (firstTry) {
        //    errorTime = tryTime;
        //    firstTry = false;
        //}else {
            CCMessageBox("网络错误!", "提示");
            errorTime = tryTime;
            tryTime *= 2;
            tryTime = std::min(tryTime, 120.0f);
        //}
    }
}



void HttpModel::update(float dt) {
    errorTime -= dt;
}


HttpModel::HttpModel(){
    //baseUrl = "http://localhost:5000/";
    
    //正式服务器
    //baseUrl = "http://172.17.0.145:91/";
    //baseRedisHost = "172.17.0.145";
 
    
    baseRedisHost = serverRedis;//ServiceCenter::getInstance()->m_serverRedis;
    
    //外网服务器
    //baseRedisHost = "113.108.228.190";
    
    //我的测试 服务器my server
    //baseUrl = "http://172.17.0.45:5000/";
    //baseRedisHost = "172.17.0.45";
}


struct TempData {
    CCObject *object;
    MyHttpResp psel;
    void *param;
};

string url_encode(string value){
    std::string src = value;
    char hex[] = "0123456789ABCDEF";
    string dst;
    for (size_t i = 0; i < src.size(); ++i)
    {
        unsigned char cc = src[i];
        if (isascii(cc))
        {
            if (cc == ' ')
            {
                dst += "%20";
            }
            else
                dst += cc;
        }
        else
        {
            unsigned char c = static_cast<unsigned char>(src[i]);
            dst += '%';
            dst += hex[c / 16];
            dst += hex[c % 16];
        }
    }
    return dst;
}
void HttpModel::addRequest(string url, string method, std::map<string, string> postData, CCObject *object, MyHttpResp psel, void *param){
    CCLog("addRequest %s %s", url.c_str(), method.c_str());
    CCHttpRequest *request = new CCHttpRequest();
    
    if (method == "POST") {
        request->setRequestType(CCHttpRequest::kHttpPost);
    } else if (method == "PUT") {
        request->setRequestType(CCHttpRequest::kHttpPut);
    } else if(method == "GET"){
        request->setRequestType(CCHttpRequest::kHttpGet);
    }
    
    request->setResponseCallback(this, httpresponse_selector(HttpModel::handleHttp));
    request->setTag("test request");
    TempData *td = new TempData();
    td->object = object;
    //请求对象之前先retain一下
    if (object != NULL) {
        object->retain();
    }
    
    td->psel = psel;
    td->param = param;
    request->setUserData(td);
    string pd;
    bool first = true;
    
    for (std::map<string, string>::iterator iter=postData.begin(); iter != postData.end(); ++iter) {
        if (!first) {
            pd = pd+"&";
        }
        pd += url_encode((*iter).first);
        pd += "=";
        pd += url_encode((*iter).second);
        first = false;
    }
    CCLog("post data is %s", pd.c_str());
    
    if (method == "POST" || method == "PUT") {
        request->setUrl((baseUrl+url).c_str());
        request->setRequestData(pd.c_str(), pd.size());
    }else {
        request->setUrl((baseUrl+url+"?"+pd).c_str());
    }
    
    
    
    CCHttpClient::getInstance()->send(request);
    request->release();
    
}
//返回一个 json 对象
void HttpModel::handleHttp(CCHttpClient *client, CCHttpResponse *resp){
    TempData *td = (TempData*)resp->getHttpRequest()->getUserData();
    CCObject *obj = td->object;
    int statusCode = resp->getResponseCode();
    char statusString[64] = {};
    std::vector<char> *buffer = resp->getResponseData();
    unsigned int i;
    string ch;
    
    
    if (!resp) {
        goto FAIL;
    }
    
    
    if (0 != strlen(resp->getHttpRequest()->getTag())) {
        CCLog("request finish %s finish", resp->getHttpRequest()->getTag());
    }
    
    
    sprintf(statusString, "HTTP Status Code: %d, tag = %s", statusCode, resp->getHttpRequest()->getTag());
    if(!resp->isSucceed()){
        //CCMessageBox("网络错误!", "提示");
        CCLog("response failed");
        CCLog("error buffer %s", resp->getErrorBuffer());
        goto FAIL;
        
    }
    
    /*
     don't print response data
    for (i = 0; i < buffer->size(); i++) {
        printf("%c", (*buffer)[i]);
    }
    printf("\n");
    */
    ch = string(buffer->begin(), buffer->end());
    
    
    if (td->object != NULL) {
        MyHttpResp cb = td->psel;
        //cb();
        ((*(td->object)).*cb)(true, ch, td->param);
        obj->release();
    }
    
    //(td->object->(*cb))(true, d, td->param);
    //(&cb)(td->object, true, d, td->param);
    delete td;
    netError = false;
    return;
    
    
//网络连接失败
FAIL:
    //尝试最大的时间小于120s 2分钟提示一次
    if (errorTime < 0) {
        CCLog("http 请求出错");
        CCMessageBox("网络错误!", "提示");
        errorTime = tryTime;
        tryTime *= 2;
        tryTime = std::min(tryTime, 120.0f);
    }
    
    CCLog("net callback fail");
    netError = true;
    if (obj != NULL) {
        MyHttpResp cb = td->psel;
        ((*(td->object)).*cb)(false, ch, td->param);
        obj->release();
    }
    delete td;
    return;
}

