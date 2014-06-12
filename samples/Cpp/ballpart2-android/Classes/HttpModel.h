//
//  HttpModel.h
//  TestCpp
//
//  Created by dozengame on 14-5-13.
//
//

#ifndef __TestCpp__HttpModel__
#define __TestCpp__HttpModel__
#include "cocos2d.h"
#include "cocos-ext.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace cocos2d;
using namespace cocos2d::extension;
using namespace std;


/*
struct Request {
    
};
*/

typedef void (CCObject::*MyHttpResp)(bool suc, std::string data, void*p);
#define MYHTTP_SEL(_SELECTOR) ((MyHttpResp)(&_SELECTOR))
//json request
class HttpModel : CCObject{
public:
    static HttpModel *getInstance();
    
    std::string baseUrl;
    std::string baseRedisHost;
    
    //std::vector<Request> requestList;
    HttpModel();
    
    void addRequest(std::string url, std::string method, std::map<std::string, std::string> postData, CCObject *object, MyHttpResp pSelector, void *param);
    //doRequest();
    void handleHttp(CCHttpClient*, CCHttpResponse*);
    
    void init(string url);
    
    bool netError;
    void update(float dt);
    
private:
    void onNetState(CCObject*);
    bool firstTry;
    
    float errorTime;
    float tryTime;
};
string url_encode(string value);
#endif /* defined(__TestCpp__HttpModel__) */
