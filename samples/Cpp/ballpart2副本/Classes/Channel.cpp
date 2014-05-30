//
//  Channel.cpp
//  TestCpp
//
//  Created by dozengame on 14-5-21.
//
//

#include "Channel.h"
#include "RedisInterface.h"
#include "Message.h"
#include "ServiceCenter.h"
#include "MyBase64.h"
#include "Match.h"
#include "Logic.h"

#include "base64.h"



//TODO: 析构函数关闭redis连接

Channel::Channel()
:cid(0)
,startTime(0)
,endTime(0)
,state(0)
{
    
}

//TODO: 定时获取数据 从 redis中 返回数据
Message* Channel::refreshMessage() {
    std::string channel;
    std::string content;
    bool res = readSubData(receive, &channel, &content);
    //CCLog("refreshMessage %d", res);
    if (res) {
        
        Message *message = new Message();
        rapidjson::Document d;
        d.Parse<0>(content.c_str());
        
        message->mtype = d["type"].GetString();
        
        if (message->mtype.compare("quit") == 0) {
            delete message;
            return NULL;
        }
        
        message->flagId = d["like_team"].GetInt();
        message->sender = d["sender"].GetInt();
        message->content = d["content"].GetString();
        
        //redis 获取的用户名 需要解码
        unsigned char *out;
        const char *ns =d["name"].GetString();
        int outLen = cocos2d::base64Decode((unsigned char*)ns, strlen((const char*)ns), &out);
        string conText = string((const char*)out, (size_t)outLen);
        free(out);
        
        message->senderName = conText;
        
        if (d["length"].IsInt()) {
            message->voiceTime = d["length"].GetInt();
        }
        
        CCLog("refresh Message is %s %s", message->mtype.c_str(), message->senderName.c_str());
        return message;
    }
    return NULL;
}



//TODO: 频道
void Channel::setData(Match *mat) {
    cid = mat->mid;
    
}

bool Channel::sendMessage(string content, int contentType) {
    //TODO: 构造Message
    UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
    User *user = us->getUser();
    /*
     int mid;
     int sender;
     string senderName;
     string mtype;
     string content;
     long long send_time;
     int flagId;
     int voiceTime;
     
     
     int uid;
     string loginName;
     string password;
     string realName;
     string phoneNumber;
     string bio;
     long long registerTime;
     */
    
    Message *message = new Message();
    message->mid = 0;
    message->sender = user->uid;
    message->senderName = user->loginName;
    
    message->content = content;
    message->send_time = 0;
    message->flagId = user->flagId;
    message->voiceTime = 0;
    
    switch (contentType) {
        case TEXT_TYPE:
        {
            message->mtype = "text";
        }
            break;
        case VOICE_TYPE:
        {
            message->mtype = "voice";
            message->voiceTime = getVoiceLength(content.c_str());
            char *data = encodeVoiceC(content.c_str());
            message->content = data;
            free(data);
        }
            break;
        case IMAGE_TYPE:
        {
            message->mtype = "image";
            int len;
            void *data = getImage(&len);
            
            if (data != NULL) {
                //NSData *data = [NSData dataWithBytes:data length:len];
                //NSString *b64 = data.base64Encoding;
                int olen;
                char *odata = base64_encode((unsigned char*)data, (size_t)len, (size_t*)&olen);
                message->content = string(odata, olen);
                free(odata);
                CCLog("copy base64 data to message content");
                
                
            }
        }
            break;
        case QUIT_TYPE:
        {
            message->mtype = "quit";
        }
            break;
        default:
            break;
    }
    //删除消息对象
    
    string doc = message->Serialize();
    //CCLog("send message %s", doc.c_str());
    CCLog("start channel send message");
    
    //反馈通过 http 发送数据
    if (cid == FEEDBACK) {
        UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
        User *user = us->getUser();
        HttpModel *hm = HttpModel::getInstance();
        std::map<string, string> postData;
        char buf[256];
        sprintf(buf, "%d", user->uid);
        postData["uid"] = buf;
        postData["type"] = message->mtype;
        postData["content"] = message->content;
        
        hm->addRequest("support/feedback", "POST", postData, NULL, NULL, NULL);
        
        delete message;
        
        return true;
    }else {
        bool res =  sendMsgC(doc.c_str(), cid);
        if (!res) {
            CCNotificationCenter::sharedNotificationCenter()->postNotification("netError");
            CCLog("频道发送 消息出错");
        }
        delete message;
        return res;
    }
}



//TODO @liyong push 将参数 ip 端口 传递下去
void Channel::listenChannel(){
    CCLog("channel listen channel ");
    startReceiveRedis(cid);
    //TODO: 定时获取数据 从 redis中 然后调用 onReceiveMessage
}



