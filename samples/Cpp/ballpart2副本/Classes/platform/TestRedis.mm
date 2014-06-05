//
//  TestRedis.m
//  HelloCpp
//
//  Created by dozengame on 14-5-12.
//
//

#import "TestRedis.h"
#include "stdio.h"
#include "document.h";
#include "stringbuffer.h"
#include "writer.h"
#include "Logic.h"
#import "ImgSelect.h"
#include "MyBase64.h"
#import <AVFoundation/AVFoundation.h>
#include "ServiceCenter.h"
#include "User.h"





void *receive = NULL;
void *sender = NULL;

void startReceiveRedis(int cid){
    
    CCLog("startReceiveRedis %d", cid);
    //释放上一个聊天室的数据 新建一个 新的 接收器
    if (receive != NULL) {
        //结束进程
        [(TestRedis*)receive stopReceiver];
        //[receive dealloc];
        receive = NULL;
    }
    //只是关闭一个频道
    if (cid == -1) {
        return;
    }
    
    receive = createRedis();
    //((TestRedis*)receive)->subchannelId = cid;
    [(TestRedis*)receive setChannel:cid];
    NSLog([NSString stringWithFormat:@"receive is %@", receive ]);
    
    connectRedis(receive);
    runSubscribe(receive);
}



@implementation TestRedis

-(void)stopReceiver{
    over = true;
}
-(void)setChannel:(int)cid{
    subchannelId = cid;
}
+(id)sharedRedis{
    static TestRedis *sh = nil;
    //static dispatch_once_t onceToken;
    //dispatch_once(&onceToken, ^{
    if (sh == nil) {
        //init Test Redis
        NSLog(@"init shared Redis");
        sh = [[self alloc] init];
    }
    
    //});
    return sh;
}


//调用自己的dealloc 方法
-(void)dealloc{
    if (redis != nil) {
        /*
        if (mythread != nil) {
            [mythread cancel];
            
            mythread = nil;
        }
        */
        //over = true;
        
        [redis release];
        redis = nil;
    }
    
    [super dealloc];
}





-(id)init{
    //connectYet = false;
    self = [super init];
    
    if (self != nil) {
        mythread = nil;
        over = false;
        redis = nil;
        self->connectSuc = false;
        self->subchannelId = -1;
        self->channelName = [NSString stringWithString:@"match"];
        self->lostConnection = false;
        needUpdateHistory = false;
        sendOver = false;
        //sendQueue = [[NSMutableArray alloc] init];
        lostTime = 0;
    }
    
    return self;
}


void *createRedis() {
    NSLog(@"create Redis");
    id cd = [[TestRedis alloc] init];
    //NSLog([NSString stringWithFormat:@"TestRedis %@", cd]);
    return cd;
}
void connectRedis(void *cd) {
    NSLog(@"connect Redis");
    
    //TestRedis *tr = (TestRedis *)cd;
    //[tr connect];
}
//使用nonblock socket 来接收数据即可
void receiveData(const char *data, unsigned int length){
    
}

/*
-(void)connectAsync{
    context = redisAsyncConnect("127.0.0.1", 6379);
    if(context->err) {
        printf("Error %s\n", context->errstr);
    }
    //redisAsyncSetConnectCallback(context, (redisConnectCallback*)conCallback);
    
}
*/
void runSubscribe(void *tc){
    NSLog(@"run subscribe");
    TestRedis *tr = (TestRedis *)tc;
    [tr createThread];
}

-(void)createThread{
    mythread = [[NSThread alloc] initWithTarget:self selector:@selector(myThreadMainMethod:) object:nil];
    //[mythread start];
    [mythread start];
}


-(void) tryConnect{
    [self connect];
    if (redis == nil) {
        return;
    }
    NSString *cmd = [NSString stringWithFormat:@"subscribe %@_%d", self->channelName, self->subchannelId];
    [redis command:cmd];
    NSLog([NSString stringWithFormat:@"subscribe %d", self->subchannelId]);
}

bool getLostTime(long long *lt) {
    return [(TestRedis*)receive getLTime: lt];
}


-(bool) getLTime:(long long *)lt {
    *lt = lostTime;
    if (needUpdateHistory) {
        needUpdateHistory = false;
        return true;
    }
    return false;
}
//接收数据的 线程 crash 掉了 重新进入
-(void)myThreadMainMethod:(id)obj{
    if (!Logic::getInstance()->inChatRoom) {
        return;
    }
    
    
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSLog(@"run receive thread");
    //Warning connect must in thread itself
    chatInfo = [[NSMutableArray alloc] init];
    /*
    [self connect];
    //connect false
    if (redis == nil) {
        return;
    }
    */
    //已经订阅了其它频道 首先 取消订阅接着 再订阅新的频道
    /*
    if (self->subchannelId != -1) {
        [redis command:@"unsubscribe"];
        self->subchannelId = -1;
    }
    */
    
    //重新启动新的订阅 关闭旧的订阅 接着 打开新的订阅频道
    //self->subchannelId = Logic::getInstance()->getCID();
    //NSLog(@"su")
    
    //NSString *cmd = [NSString stringWithFormat:@"subscribe %@_%d", self->channelName, self->subchannelId];
    //[redis command:cmd];
    //NSLog([NSString stringWithFormat:@"subscribe %d", self->subchannelId]);
    
    [self tryConnect];
    NSLog(@"start thread");
    NSLog([NSString stringWithFormat:@"%d", subchannelId]);
    
    
    while (!over) {
        
        if (redis == nil) {
            [self connect];
            if (redis == nil) {
                sleep(10);
                continue;
            //通知上层获得历史消息
            //上层拿到连接断开的时间获取消息
            } else {
                //[redis retain];
                needUpdateHistory = true;
                
                NSString *cmd = [NSString stringWithFormat:@"subscribe %@_%d", self->channelName, self->subchannelId];
                [redis command:cmd];
                NSLog([NSString stringWithFormat:@"new subscribe %d", self->subchannelId]);
                
                //等待上层获取历史消息之后开始 获取立即信息
                NSLog(@"wait for top update history message then receive new message");
                sleep(3);
            }
        }
        
        NSLog(@"start receive new message");
        //如果不再聊天室则 不用接收消息
        //subscribe的time out 应该不去设定的
        if (!Logic::getInstance()->inChatRoom) {
            NSLog(@"quit chat room");
            
            break;
        }
        
        id retVal = [redis getReply];
        //服务器关闭的消息是什么 格式的 retVal不是这样
        NSLog(@"replay is");
        if ([retVal class] == [NSArray class]) {
            NSLog(@"array");
        }else if([retVal class] == [NSNull class]) {
            NSLog(@"null retval");
        }else {
            NSLog([NSString stringWithFormat:@"%@", [retVal class]]);
        }
        //NSLog([NSString stringWithFormat:@"%@", retVal ]);
        
        //NSLog(typeof(retVal));
        //NSLog([NSString stringWithFormat:@"%@", retVal]);
        
        if (over) {
            NSLog(@"thread over ");
            break;
        }
        
        //连接断开了
        if (retVal == nil) {
            //NSLog(@"和聊天服务器连接断开了 重新尝试连接 首先释放之间的连接对象");
            NSLog(@"error connect");
            //重新连接 尝试连接服务器 接收消息
            if(redis != nil) {
                [redis dealloc];
                redis = nil;
            }
            if (!lostConnection) {
                lostConnection = true;
                time_t t;
                time(&t);
                lostTime = t;
                NSLog(@"lost connection");
                NSLog([NSString stringWithFormat:@"lostTime %lld", lostTime]);
            
            }
        } else {
            NSLog(@"push into chat info message");
            @synchronized(chatInfo){
                [chatInfo addObject:retVal];
            }
        }
        NSLog(@"finish reply");
        
    }
    
    //sleep(5);
    
    NSLog(@"close receive thread");
    NSLog([NSString stringWithFormat:@"%d", subchannelId]);
    
    //线程清理 有问题？
    //锁屏之后 再退出
    
    //TODO:: 释放存储池子 和 自身有问题？
    [chatInfo release];
    
    [pool release];
    
    //退出聊天场景删除自身对象 为什么会 crash
    //线程 内部分配的对象 如何删除掉呢？
    [self release];
}

bool readSubData(void *c, std::string* cha, std::string *con){
    TestRedis *tr = (TestRedis*)c;
    const char*ca, *cn;
    BOOL has = [tr readData:&ca c:&cn];
    //NSLog([NSString stringWithFormat:@"readSubData %d", has]);
    if (has) {
        NSLog(@"read from chatinfo");
        cha->assign(ca);
        con->assign(cn);
    }
    if (has == YES) {
        return true;
    }else
        return false;
    
}
-(BOOL) readData:(const char**)cha c:(const char**)con{
    id msg = nil;
    @synchronized(chatInfo){
        if ([chatInfo count] > 0) {
            msg = [[chatInfo objectAtIndex:0] retain];
            [chatInfo removeObjectAtIndex:0];
        }
    }
    if(msg != nil) {
        NSString *channel = [msg objectAtIndex:1];
        NSString *content = [msg objectAtIndex:2];
        *cha = [channel UTF8String];
        *con = [content UTF8String];
        [msg release];
        return YES;
    }
    return NO;
}

//当未连接的时候连接服务器
//当连接失败 返回NULL
void *connect(){
    TestRedis *tr = [TestRedis sharedRedis];
    if (tr->redis != nil) {
        return tr;
    }
    [tr connect];
    if (tr->redis == nil) {
        return NULL;
    }
    return tr;
}


//阻塞连接 会block掉整个程序 需要使用 Aynchronized 来做连接 或者 采用多线程 来做连接
-(void) connect{
    NSLog(@"connect to redis server");
    
    NSString *h = [NSString stringWithCString:HttpModel::getInstance()->baseRedisHost.c_str()];
    redis = [ObjCHiredis redis:h on:[NSNumber numberWithInt:6379]];
    [redis retain];
    if (redis == nil) {
        NSLog([NSString stringWithFormat:@"redis connection fail %@", h]);
        self->connectSuc = false;
    }else {
        NSLog([NSString stringWithFormat:@"redis connection success %@", h]);
        self->connectSuc = true;
    }
    
    /*
    id key = [redis command:@"set testKey 123"];
    [redis command:@"subscribe chat"];
    id retVal = [redis getReply];
    NSLog(@"testRedis" );
    NSLog([NSString stringWithFormat:@"%@", key]);
    NSLog([NSString stringWithFormat:@"%@", retVal]);
    
    
    key = [redis command:@"get testKey"];
    retVal = [redis getReply];
    NSLog(@"key is");
    NSLog([NSString stringWithFormat:@"%@", key]);
    NSLog([NSString stringWithFormat:@"%@", retVal]);
    */
}
char *encodeVoiceC(const char*fn) {
    return [[TestRedis sharedRedis] encodeVoice:fn];
}
-(char*)encodeVoice:(const char*)fn{
    NSFileManager *fmr = [NSFileManager defaultManager];
    NSData *db = [fmr contentsAtPath:[NSString stringWithFormat:@"%s", fn]];
    //data and length different
    //const void* data = db.bytes;
    //NSInteger len = db.length;
    NSString *b64 = db.base64Encoding;
    const char* f = [b64 UTF8String];
    char *ret = (char*)malloc((strlen(f)+1)*sizeof(char));
    memcpy(ret, f, strlen(f)+1);
    return ret;
}

-(void)sendVoice:(const char *)fn{
    NSLog(@"send voice file");
    //NSLog([NSString stringWithFormat:@"%@", url ]);
    //const char *fn = [url fileSystemRepresentation];
    NSLog([NSString stringWithFormat:@"filename %s", fn ]);
    //FILE *f = fopen(fn, "r");
    //connection Fail or connection lost
    if (redis == nil) {
        return;
    }
    
    int vl = getVoiceLength(fn);
    
    NSFileManager *fmr = [NSFileManager defaultManager];
    NSData *db = [fmr contentsAtPath:[NSString stringWithFormat:@"%s", fn]];
    //data and length different
    //const void* data = db.bytes;
    //NSInteger len = db.length;
    NSString *b64 = db.base64Encoding;
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType &allocator = d.GetAllocator();
    d.AddMember("type", "voice", allocator);
    d.AddMember("content", [b64 UTF8String], allocator);
    
    d.AddMember("sender", Logic::getInstance()->getSenderId(), allocator);
    d.AddMember("like_team", Logic::getInstance()->getFlagId(), allocator);
    d.AddMember("length", int(vl), allocator);
    
    
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    d.Accept(writer);
    
    UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
    User *user = us->getUser();
    
    int mid = user->uid;
    
    NSString *cmd = [NSString stringWithFormat:@"publish %@_%d %s", self->channelName,  mid, strbuf.GetString()];
    NSLog(@"send cmd");
    //NSLog(cmd);
    NSLog([NSString stringWithFormat:@"%d cmd %d", (int)b64.length, (int)cmd.length]);
    id retVal = [redis command:cmd];
    //id retVal = [redis getReply];
    NSLog([NSString stringWithFormat:@"retval %@", retVal]);
    NSLog(@"finish read");
}



-(void) sendImg{
    if (redis == NULL) {
        return;
    }
    NSLog(@"send Img to Server");
    int len;
    void *data = getImage(&len);
    if (data != NULL) {
        //NSData *data = [NSData dataWithBytes:data length:len];
        //NSString *b64 = data.base64Encoding;
        int olen;
        char *odata = base64_encode((unsigned char*)data, (size_t)len, (size_t*)&olen);
        std::string rd = string(odata, olen);
        free(odata);
        
        //NSString *b64 = [data base64EncodedDataWithOptions:nil];
        
        rapidjson::Document d;
        d.SetObject();
        rapidjson::Document::AllocatorType  &allocator = d.GetAllocator();
        d.AddMember("type", "image", allocator);
        d.AddMember("content", rd.c_str(), allocator);
        
        d.AddMember("sender", Logic::getInstance()->getSenderId(), allocator);
        d.AddMember("like_team", Logic::getInstance()->getFlagId(), allocator);
        
        
        rapidjson::StringBuffer strbuf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
        d.Accept(writer);
        
        //int mid = Logic::getInstance()->getCID();
        int mid = 0;
        NSString *cmd = [NSString stringWithFormat:@"publish %@_%d %s", self->channelName, mid, strbuf.GetString()];
        NSLog(@"send cmd");
        //NSLog(cmd);
        NSLog([NSString stringWithFormat:@"%d cmd %d", (int)rd.size(), (int)cmd.length]);
        id retVal = [redis command:cmd];
        //id retVal = [redis getReply];
        NSLog([NSString stringWithFormat:@"retval %@", retVal]);
        NSLog(@"finish read");
        
    }
}

void startSend(const char *fn){
    TestRedis *tr = [TestRedis sharedRedis];
    if (tr->redis == nil) {
        [tr connect];
    }
    [tr sendVoice:fn];
}

void sendImage(){
    TestRedis *tr = [TestRedis sharedRedis];
    if(tr->redis == nil) {
        [tr connect];
    }
    [tr sendImg];
}


void sendText(std::string text){
    TestRedis *tr = [TestRedis sharedRedis];
    if (tr->redis == nil) {
        [tr connect];
    }
    //rapidjson::Document d;
    
    [tr redisSendText:text.c_str()];
}

//发送命令也要一个异步的线程来处理才行
-(void)redisSendText:(const char *)text{
    if (redis == nil) {
        connectSuc = false;
        return;
    }
    //NSString *s = [NSString stringWithFormat:@""];
    Logic *log = Logic::getInstance();
    //int mid = log->getCID();
    int mid = 0;
    NSLog([NSString stringWithFormat:@"channel is %d  text ", mid]);
    NSString *cmd = [NSString stringWithFormat:@"publish %@_%d %s", self->channelName, mid, text];
    NSLog(cmd);
    //NSLog(cmd);
    //[redis retainCount];
    
    NSLog([NSString stringWithFormat:@"%d", [redis retainCount]]);
    id retVal = [redis command:cmd];
    NSLog([NSString stringWithFormat:@"retval %@", retVal]);
    
    NSLog(@"send Text suc");
}
bool sendMsgC(const char*msg, int mid, int msgId) {
    //return [[TestRedis sharedRedis] sendMsg:msg m:mid];
    if (sender != NULL) {
        return [(TestRedis*)sender sendMsg:msg m:mid mi:msgId];
    }
    return false;
}


//游戏锁屏之后 redis socket 连接 出现问题 需要 重新连接一下  resumeGame
void reconnectRedis() {
    [[TestRedis sharedRedis] reconnect];
}



-(void)reconnect{
    if (redis != nil) {
        [redis dealloc];
        redis = nil;
    }
}

/*
void closeSendRedisC() {
    [[TestRedis sharedRedis] closeSendRedis];
}
*/


-(void) closeSendRedis{
    NSLog(@"close send redis when quit chatView or enter chat info");
    if (redis != nil) {
        [redis release];
        redis = nil;
    }
}

//初始化发送线程
-(void)initSendThread{
    NSLog(@"initSendThread");
    [NSThread detachNewThreadSelector:@selector(sendThread) toTarget:self withObject:nil];
}

void startSendRedis(int cid) {
    NSLog(@"startSendRedis");
    
    //释放上一个聊天室的数据 新建一个 新的 接收器
    if (sender != NULL) {
        //结束进程
        @synchronized(sender) {
            [(TestRedis*)sender stopSend];
            //[receive dealloc];
            sender = NULL;
        }
    }
    
    //只是关闭一个频道 发射者
    if (cid == -1) {
        return;
    }
    
    sender = [[TestRedis alloc] init];
    [(TestRedis*)sender setChannel:cid];
    NSLog([NSString stringWithFormat:@"sender is %@", sender ]);
    [(TestRedis *)sender initSendThread];
}

//所有频道实用 一个 公共的发送线程
//建立新的对象 来发送消息
-(void) stopSend {
    //线程已经结束 则 释放自身
    if (sendOver) {
        [self release];
    //线程未结束则 设置要释放自身
    }else
        sendOver = true;
}
struct MsgObj {
    int msgId;
    NSString *msg;
};


int getMsgState(int msgId){
    if (sender != NULL) {
        @synchronized(sender) {
            return Logic::getInstance()->getMsgState(msgId);
        }
    }
    
    return -1;
}

-(void)sendThread {
    NSAutoreleasePool *pool;
    @synchronized(self) {
        pool = [[NSAutoreleasePool alloc] init];
    }
    NSLog(@"run send thread");
    //Warning connect must in thread itself
    
    @synchronized(self) {
        sendQueue = [[NSMutableArray alloc] init];
        [self connect];
        
        //设置发送超时 时间
        [self->redis setTimeout];
    }
    
    
    
    //网络连接断开 上层重新连接 发起请求
    while (!sendOver) {
        if (redis == nil) {
            /*
            @synchronized(self) {
                [self connect];
                [self->redis setTimeout];
            }
            sleep(5);
            
            continue;
             */
            break;
        }
        
        
        NSString *msg = NULL;
        //struct MsgObj p;
        @synchronized(self){
            if (sendQueue.count > 0) {
                //[[sendQueue objectAtIndex:0] getValue:&p];
                //msg = p.msg;
                //[sendQueue removeObjectAtIndex:0];
                msg = [sendQueue objectAtIndex:0];
                [msg retain];
                [sendQueue removeObjectAtIndex:0];
                
            }
        }
        if (sendOver) {
            if (msg != NULL) {
                [msg release];
                msg = NULL;
            }
            break;
        }
        
        //sendMsg 设定频道
        //当前所在频道 发送频道消息
        if (msg != NULL) {
            NSLog(@"try send message");
            NSString *cmd = [NSString stringWithFormat:@"publish %@_%d %@", self->channelName, self->subchannelId, msg];
            //分配的msg 空间释放掉
            //free(msg);
            [msg release];
            NSLog(@"send commd");
            //NSLog(cmd);
            
            id retVal;
            //@synchronized(self) {
            //阻塞性发送不能 加锁 否则 主线程也会被卡住
            retVal = [redis command:cmd];
            //}
            if (retVal == nil) {
                NSLog(@"send Message error reconnect");
                
                [redis dealloc];
                redis = nil;
                //return false;
                //发送失败
                
                //TODO: 加锁 访问 Logic 中得map数据对象
                @synchronized(self){
                    //Logic::getInstance()->setMsgState(p.msgId, 2);
                }
            }else {
                //加锁访问状态
                //发送成功
                @synchronized(self) {
                    //Logic::getInstance()->setMsgState(p.msgId, 1);
                }
            }
            NSLog([NSString stringWithFormat:@"send Message over %@", retVal]);
        }else{
            //block 或者睡眠
            sleep(1);
        }
    }
    
    //等待最后一个消息发送出去 再 退出线程
    //sleep(5);
    
    //释放未发送出去的消息
    
    //struct MsgObj p;
    @synchronized(self) {
        NSLog(@"message count");
        NSLog([NSString stringWithFormat:@"%d", sendQueue.count ]);
        
        /*
        for(int i=0; i < sendQueue.count; i++){
            //[[sendQueue objectAtIndex:i] getValue:&p];
            
            //message 已经被释放了么？
            //NSLog(@"message count %d", [p.msg retainCount]);
            
            [p.msg release];
        }
         */
    }
    
    
    NSLog(@"clear send Queue");
    @synchronized(self) {
        [sendQueue release];
        sendQueue = nil;
    }
    NSLog(@"close Send thread");
    
    NSLog([NSString stringWithFormat:@"error state %d", sendOver]);
    
    //@synchronized(self) {
    [pool release];
    
    //用户退出聊天室 则自己清理 自己
    //发送中网络断开 仍在聊天室中 则 设置sendOver状态 通知上层释放
    @synchronized(self) {
        if (sendOver) {
            [self release];
            //sender = NULL;
        }else {
            sendOver = true;
        }
    }
        //[self release];
    //}
}


//将消息压入到缓冲区中
-(bool)sendMsg:(const char *)msg m:(int)mid mi:(int) msgId{
    //没有连接直接返回 失败即可 不用压入后续的message了
    //连接已经断开了 释放对象 重新连接
    if (sendOver) {
        NSLog(@"can't send message");
        //重新连接 准备发送消息
        startSendRedis(subchannelId);
        return false;
    }
    
    if (redis == nil ) {
        NSLog(@"send lost connection so return false");
        return false;
    }
    
    
    NSLog(@"sendMessage why connect yet?");
    
    //正在发送 设置状态
    //1 成功了
    @synchronized(self) {
        Logic::getInstance()->setMsgState(msgId, 1);
    }
    
    
    //1 成功
    //2 失败
    //struct MsgObj obj;
    //obj.msgId = msgId;
    //obj.msg = [NSString stringWithUTF8String:msg ];
    //[obj.msg retain];
    
    //不应该加入到 autorelease池子中的
    //obj.msg = [[NSString alloc] initWithUTF8String:msg];
    
    if (sendQueue != nil) {
        @synchronized(self){
            if (sendQueue != nil) {
                //[sendQueue addObject:[NSValue value:&obj withObjCType:@encode(struct MsgObj) ]];
                NSString *nmsg = [[NSString alloc] initWithUTF8String:msg];
                [sendQueue addObject:nmsg];
                [nmsg release];
                
            }

        }
    }else {
        //[obj.msg release];
    }
    
    return true;
}

/*
-(bool)sendMsg:(const char *)msg m:(int)mid{
    NSLog(@"sendMessage why connect yet?");
    
    if (redis == nil) {
        [self connect];
    }
    if (redis == nil) {
        connectSuc = false;
        return false;
    }
    NSLog(@"sendMsg why error but block ? time out too long?");
    
    NSLog([NSString stringWithFormat:@"send Msg is %d", mid]);
    NSString *cmd = [NSString stringWithFormat:@"publish %@_%d %s", self->channelName, mid, msg];
    //NSLog(cmd);
    id retVal = [redis command:cmd];
    if (retVal == nil) {
        NSLog(@"send Message error reconnect");
        [redis dealloc];
        redis = nil;
        return false;
    }
    
    NSLog([NSString stringWithFormat:@"retval %@", retVal]);
    NSLog(@"send Msg suc");
    
    return  true;
}
*/


@end
