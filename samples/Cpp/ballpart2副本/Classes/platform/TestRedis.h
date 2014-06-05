//
//  TestRedis.h
//  HelloCpp
//
//  Created by dozengame on 14-5-12.
//
//

#import <Foundation/Foundation.h>
#include "RedisInterface.h"
#import "ObjCHiredis.h"
//#import "async.h"

@interface TestRedis : NSObject {
    ObjCHiredis *redis;
    
    //redisAsyncContext *context;
    NSMutableArray *chatInfo;
    //bool connectYet;
    bool connectSuc;
    int subchannelId;
    NSString *channelName;
    
    bool lostConnection;
    long long lostTime;
    
    bool needUpdateHistory;
    bool over;
    
    NSThread *mythread;
    
    NSMutableArray *sendQueue;
    
    
    bool sendOver;
}
-(void) reconnect;
-(void) stopReceiver;
-(id) init;
-(void) connect;
-(void) sendVoice:(const char *)fn;
-(void) sendImg;
-(char*) encodeVoice:(const char*)fn;
-(void) setChannel:(int) cid;

-(void)dealloc;

+(id)sharedRedis;

//-(void) connectAsync;

-(void) createThread;

-(void) myThreadMainMethod:(id )obj;
-(BOOL) readData:(const char**)cha c:(const char**)con;

-(void) redisSendText:(const char *)text;
-(void) tryConnect;

-(bool) sendMsg:(const char*)msg m:(int)mid mi:(int)msgId;
-(bool) getLTime:(long long *)lt;

-(void) closeSendRedis;

-(void) initSendThread;

-(void) sendThread;
-(void) stopSend;

//-(void) sendThread;
@end
