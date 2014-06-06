//
//  RedisInterface.h
//  HelloCpp
//
//  Created by dozengame on 14-5-12.
//
//

#ifndef HelloCpp_RedisInterface_h
#define HelloCpp_RedisInterface_h
#include "async.h"
#include "string"
void *connect();

void *startRecord();
void *stopRecord();
void startPlay();
void startSend(const char*);
void sendText(std::string);
void sendImage();

std::string getFileName();



void *createRedis();
void connectRedis(void *);
void runSubscribe(void *);
bool readSubData(void *, std::string *, std::string *);
//limit data Size
//void receiveData(const char*, unsigned int length);
//void conCallback(redisAsyncContext *, void *);

extern void *receive;
extern void *sender;


std::string storeFile(const unsigned char*con, int len, int vid);
void playVoice(int vid);

void startReceiveRedis(int cid);
void startSendRedis(int cid);

void openImageSelect();

void *getImage(int *length);

bool checkGetYet();

int getMyRecordVid();
int getVoiceLength(const char *);

char *encodeVoiceC(const char *fn);


bool sendMsgC(const char*, int mid, int msgId);

bool getLostTime(long long *lostTime);

//void reconnectRedis();


//void closeSendRedisC();

void testUtf8C(const char*);
int getMsgState(int msgId);

#endif
