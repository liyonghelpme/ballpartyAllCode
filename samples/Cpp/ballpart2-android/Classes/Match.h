//
//  Match.h
//  TestCpp
//
//  Created by dozengame on 14-5-21.
//
//

#ifndef __TestCpp__Match__
#define __TestCpp__Match__

#include <iostream>
#include <string.h>
#include "cocos2d.h"
#include "HttpModel.h"
#include "Match.h"

using namespace std;

class Match : public CCObject{
public:
    //TODO: @liyong 实现这个类
    int mid;
    string cate_name;
    string title;
    string host_name;
    string guest_name;
    long long start_time;
    long long end_time;
    
    //0 完场 1 进行 2 未开始
    int state;
    
    string result;
    string comment;
    
    //Message 实现消息
    //void setData(Match*);
    void setData(rapidjson::Value &data);
    
    int recaculateState();
};






#endif /* defined(__TestCpp__Match__) */
