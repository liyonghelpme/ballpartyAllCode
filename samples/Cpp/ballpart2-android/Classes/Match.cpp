//
//  Match.cpp
//  TestCpp
//
//  Created by dozengame on 14-5-21.
//
//

#include "Match.h"
//TODO: 获取比赛数据
void Match::setData(rapidjson::Value &data) {
    mid = data["id"].GetInt();
    start_time = data["start_time"].GetInt64();
    end_time = data["end_time"].GetInt64();
    cate_name = data["cate_name"].GetString();
    title = data["title"].GetString();
    host_name = data["host_name"].GetString();
    guest_name = data["guest_name"].GetString();
}

int Match::recaculateState() {
    time_t now;
    time(&now);
    long long st = start_time/1000;
    long long et = end_time/1000;
    int curstate = 0;
    //比赛已经结束
    //CCLog("now end_time start_time %llf %llf %llf", (double)now, (double)start_time, (double)end_time);
    
    //0 完场
    //1 正在进行
    //2 未开始
    if (now >= et) {
        curstate = 0;
        //curstate = 0;
        //state = 1 比赛正在进行
    } else if(now >= st) {
        curstate = 1;
    } else {
        curstate = 2;
    }
    return curstate;
}

