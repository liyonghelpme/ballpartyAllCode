//
//  Message.cpp
//  TestCpp
//
//  Created by dozengame on 14-5-21.
//
//


#include "Message.h"
#include "HttpModel.h"
#include "Logic.h"
#include "Writer.h"
#include "stringbuffer.h"
#include "base64.h"
#include "MyBase64.h"

//序列化消息
string Message::Serialize(){
    rapidjson::Document d;
    d.SetObject();
    
    rapidjson::Document::AllocatorType &allocator = d.GetAllocator();
    d.AddMember("type", mtype.c_str(), allocator);
    d.AddMember("content", content.c_str(), allocator);
    d.AddMember("sender", sender, allocator);
    d.AddMember("like_team", flagId, allocator);
    d.AddMember("length", voiceTime, allocator);
    
    
    //名字做base64 编码 发送
    int olen;
    char *odata = base64_encode((const unsigned char*)senderName.c_str(), (size_t)senderName.size(), (size_t*)&olen);
    std::string od = string(odata, olen);
    free(odata);
    d.AddMember("name", od.c_str(), allocator);
    
    
    
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    //没有对中文做utf8编码操作
    d.Accept(writer);
    
    return strbuf.GetString();
}

//获取历史消息 数据
void Message::setData(const rapidjson::Value &b) {
    sender = b["user_id"].GetInt();
    mtype = b["type"].GetString();
    if (b["like_team"].IsInt()) {
        flagId = b["like_team"].GetInt();
    }else
        flagId = 0;
    
    content = b["content"].GetString();
    if (b["real_name"].IsString()) {
        senderName = b["real_name"].GetString();
    }
    if (b["length"].IsInt()) {
        voiceTime = b["length"].GetInt();
    }
    
}

void Message::UnSerialize(string &con, string &type) {
    
}