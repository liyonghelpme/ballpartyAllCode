#include "RedisInterface.h"
#include "pthread.h"
#include "hiredis.h"
#include "Logic.h"
#include "HttpModel.h"
#include <unistd.h>

#include <vector>

using namespace std;

void *receive = NULL;
void *sender = NULL;

pthread_mutex_t sender_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t receive_mutex = PTHREAD_MUTEX_INITIALIZER;


class TestRedis;
void *sendThread(void *);
void *receiveThread(void *);


class TestRedis {
private:
	bool sendOver, initConnectYet, over, needUpdateHistory, lostConnection;
	redisContext *redis;
	std::vector<string*> sendQueue;
	string channelName;
	int subchannelId;
	std::vector< std::vector<redisReply*> * > chatInfo;
	long long lostTime;
public:
	TestRedis()
	:sendOver(false)
	,initConnectYet(false)
	,over(false)
	,needUpdateHistory(false)
	,lostConnection(false)
	,redis(NULL)
	,channelName("match")
	,subchannelId(-1)
	,lostTime(0)
	{

	}
	
	~TestRedis() {
		if (redis != NULL)
		{
			redisFree(redis);
			redis = NULL;
		}
	}
	void parseReply(redisReply *reply){

	}

	//前两个消息已经被读取了 其余消息释放掉
	//可能会漏掉消息么?
	void removeMsg(std::vector<redisReply*> *msg){
		for(int i=0; i < msg->size(); i++) {
			//CCLog("error you have more message to read");
			
			redisReply *rp = (*msg)[i];
			int type = rp->type;
			CCLog("rm msg type is %d", type);
			CCLog(" ele %d", rp->elements);

			if(type == REDIS_REPLY_STRING) {
				CCLog("%s", rp->str);
			}

			freeReplyObject(rp);
		}
		CCLog("delete message object");

		delete msg;
		CCLog("delete over ");
	}

	bool readData(string**cha, string **con) {
		std::vector<redisReply*> *msg = NULL;
		pthread_mutex_lock(&receive_mutex);
		if (chatInfo.size() > 0)
		{
			msg = chatInfo[0];
			chatInfo.erase(chatInfo.begin());
		}
		pthread_mutex_unlock(&receive_mutex);

		if(msg != NULL) {
			CCLog("message length %d", msg->size());
			if (msg->size() == 1)
			{
				redisReply *reply = (*msg)[0];
				//两个 响应的返回数据
				CCLog("msg reply is %d %d", reply->type, reply->elements);
				if (reply->type == REDIS_REPLY_ARRAY)
				{
				
					if (reply->elements == 3)
					{
						/*
						for(int i = 0; i < reply->elements; i++){
							redisReply *rp = reply->element[i];
							CCLog("id %d type %d", i, rp->type);
							if(rp->type == REDIS_REPLY_STRING) {
								CCLog("msg %s", rp->str);
							}
						}
						*/
						

						redisReply *m0 = reply->element[0];
						redisReply *m1 = reply->element[1];
						redisReply *m2 = reply->element[2];

						if(m1->type == REDIS_REPLY_STRING){
							*cha = new string(m1->str);
						}
						if(m2->type == REDIS_REPLY_STRING){
							*con = new string(m2->str);
						}
						CCLog("messsage suc %x %x", *cha, *con);
						removeMsg(msg);
						CCLog("remove msg over");
						return true;
					}
				}
			}

			CCLog("not right message ");
			removeMsg(msg);
			/*
			if(msg->size() == 2) {
				redisReply *reply = (*msg)[0];
				if (reply->type == REDIS_REPLY_STRING)	
				{
					*cha = new string(reply->str);
				}else {
					*cha = NULL;
				}
				//freeReplyObject(reply);

				reply = (*msg)[1];
				if (reply->type == REDIS_REPLY_STRING)
				{
					*con = new string(reply->str);
				}else {
					*con = NULL;
				}
				CCLog("read message suc");
				//freeReplyObject(reply);
				removeMsg(msg);
				return true;
			}
			//完全释放消息体
			removeMsg(msg);
			*/

			CCLog("message length not enough");
			
		}else {

		}

		return false;
	}

	void stopReceiver() {
		over = true;
	}
	redisReply *command(const string *msg) {
		redisReply *reply = (redisReply*)redisCommand(redis, msg->c_str());
		if(reply == NULL) {
			CCLog("command error %s", msg->c_str());
			return NULL;
		}
		//接受者要释放reply对象
		return reply;
	}
	vector<redisReply*> *arrayFromVector(redisReply *reply) {
		std::vector<redisReply*> *replies = new std::vector<redisReply*>();
		for(int i = 0; i < reply->elements; i++) {
			replies->push_back(reply->element[i]);
		}
		return replies;
	}
	//返回一组 reply 需要调用者 释放 每个reply 和 整个vector
	std::vector<redisReply*> *getReply(){
		std::vector<redisReply*> *replies = new std::vector<redisReply*>();


		void *aux = NULL;
		int ret = redisGetReply(redis, &aux);
		if (ret == REDIS_ERR)
		{
			delete replies;
			return NULL;
		}
		if (aux == NULL)
		{
			int wdone = 0;
			while(!wdone) {
			    if(redisBufferWrite(redis, &wdone) == REDIS_ERR){
			    	delete replies;
			    	return NULL;
			
			    }
			}
			while(redisGetReply(redis, &aux) == REDIS_OK){
				redisReply *reply = (redisReply*)aux;
				CCLog("reply2 type is %d", reply->type);
				replies->push_back(reply);
			}

		}else {
			redisReply *reply = (redisReply*)aux;
			CCLog("reply type is %d", reply->type);
			//将数组转化成 replies
			/*
			if (reply->type == REDIS_REPLY_ARRAY)
			{
				delete replies;
				replies = arrayFromVector(reply);
			}
			*/
			replies->push_back(reply);
		}

		CCLog("getReply is %d", replies->size());
		return replies;
	}

	void stopSend() {
		sendOver = true;

	}
	
	void setChannel(int cid) {
		subchannelId = cid;
	}

	void initSendThread() {
		CCLog("initSendThread");

		pthread_t tid;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&tid, &attr, sendThread, this);
		
	}

	void runSubscribe() {
		pthread_t tid;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&tid, &attr, receiveThread, this);
	}

	void connect() {
		CCLog("connect redis");

		struct timeval tv;
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		const char* h = HttpModel::getInstance()->baseRedisHost.c_str();

		CCLog("host is %s", h);
		redis = redisConnectWithTimeout(h, redisPort, tv);
		CCLog("connect result is %x", redis);

		if (redis->err != 0)
		{
			CCLog("%s", redis->errstr);
			redisFree(redis);
			redis = NULL;
		}else {
			CCLog("connect success!");
		}
	}

	bool sendMsg(const char *msg, int mid, int msgId) {
		if(sendOver) {
			startSendRedis(subchannelId);
			return false;
		}
		if (redis == NULL && initConnectYet)
		{
			return false;
		}
		
		if (!initConnectYet)
		{
			return false;
		}


		pthread_mutex_lock(&sender_mutex);
		Logic::getInstance()->setMsgState(msgId, 1);
		string *smsg = new string(msg);
		sendQueue.push_back(smsg);
		pthread_mutex_unlock(&sender_mutex);

		return true;
	}
	void tryConnect() {
		CCLog("tryConnect  subscribe");
		connect();
		if (redis == NULL)
		{
			return;
		}

		string cmd = "subscribe ";
		char buf[256];
		sprintf(buf, "%s_%d", channelName.c_str(), subchannelId);
		cmd += buf;

		redisReply *retVal = command(&cmd);
		CCLog("finish subscribe %x", retVal);
		if (retVal == NULL)
		{	
			CCLog("sbuscribe error");
			redisFree(redis);
			redis = NULL;
		}else {
			CCLog("subscribe succ");
		}
	}

	void tReceiveThread() {
		CCLog("receive thread try get");
		tryConnect();
		while(!over) {
		    if (redis == NULL)
		    {
		    	tryConnect();

		 		if (redis == NULL)
		 		{
		 		   	usleep(10);
		 		   	continue;
		 		}else {
		 			//等待3s 上层拉去 历史聊天信息
		 			needUpdateHistory = true;
		 			sleep(3);
		 		}   
		    }

		    if (over)
		    {
		    	break;
		    }
		    std::vector<redisReply*> *retVal = getReply();
		    if (retVal == NULL)
		    {
		    	if (redis != NULL)
		    	{
		    		redisFree(redis);
		    		redis = NULL;
		    	}
		    	if (!lostConnection)
		    	{
		    		lostConnection = true;
		    		time_t t;
		    		time(&t);
		    		lostTime = t;
		    		CCLog("receive connection lost");
		    	}
		    }else {
		    	pthread_mutex_lock(&receive_mutex);
		    	chatInfo.push_back(retVal);
		    	pthread_mutex_unlock(&receive_mutex);
		    }
		}
		pthread_mutex_lock(&receive_mutex);
		for (int i = 0; i < chatInfo.size(); ++i)
		{
			std::vector<redisReply*> *rp = chatInfo[i];
			for (int j = 0; j < rp->size(); ++j)
			{
				freeReplyObject((void*)(*rp)[j]);
			}
			delete rp;
		}
		chatInfo.clear();
		pthread_mutex_unlock(&receive_mutex);

		delete this;

		CCLog("close receive thread clear chatInfo");
	}
	void tSendThread() {
		CCLog("tSendTrhead start");

		pthread_mutex_lock(&sender_mutex);
		connect();
		
		pthread_mutex_unlock(&sender_mutex);	

		if (redis != NULL)
		{
			struct timeval tv;
			tv.tv_sec = 3;
			tv.tv_usec = 0;
			redisSetTimeout(redis, tv);
			initConnectYet = true;	
		}
		

		char buf[256];

		while(!sendOver) {
		    if (redis == NULL)
		    {
		    	break;
		    }
		    std::string *msg = NULL;
		    pthread_mutex_lock(&sender_mutex);
		    if (sendQueue.size() > 0)
		    {
		    	msg = sendQueue[0];
		    	sendQueue.erase(sendQueue.begin());
		    }
		    pthread_mutex_unlock(&sender_mutex);

		    if (sendOver)
		    {
		    	if (msg != NULL)
		    	{
		    		delete msg;
		    		msg = NULL;
		    	}
		    	break;
		    }
		    if(msg != NULL) {
		    	string cmd = "publish ";
		    	cmd += channelName+"_";
		    	sprintf(buf, "%d", subchannelId);
		    	cmd += buf;
		    	cmd += " ";
		    	cmd += *msg;
		    	delete msg;

		    	redisReply *retVal= NULL;
		    	retVal = command(&cmd);

		    	if (retVal == NULL)
		    	{
		    		redisFree(redis);
		    		redis = NULL;
		    	}else {
		    		freeReplyObject(retVal);
		    	}
		    }else {
		    	sleep(1);
		    }
		}

		pthread_mutex_lock(&sender_mutex);
		for(int i = 0; i < sendQueue.size(); i++) {
			string *msg = sendQueue[i];
			delete msg;
		}
		sendQueue.clear();

		//当退出聊天室 需要关闭线程的时候 调用
		if(sendOver) {
			delete this;
		//当聊天连接断开的时候需要通知外部
		}else {
			sendOver = true;
		}

		pthread_mutex_unlock(&sender_mutex);


	}

	bool getLTime(long long *t){
		*t = lostTime;
		if (needUpdateHistory)
		{
			needUpdateHistory = false;
			return true;
		}
		return false;
	}

};

void *receiveThread(void *tr) {
	CCLog("receiveThread %x", tr);
	TestRedis *tt = (TestRedis*)tr;
	tt->tReceiveThread();
	return NULL;
}

void *sendThread(void *tr) {
	CCLog("send Thread start %x", tr);

	TestRedis *tt = (TestRedis*)tr;
	tt->tSendThread();

	return NULL;
}

/*
void *connect(){

}
*/


/*
void startPlay(){

}


void startSend(const char*){

}

void sendText(std::string s) {

}


void sendImage() {

}



void *createRedis() {

}

void connectRedis(void *r) {

}
*/


bool readSubData(void *r, std::string *cha, std::string *con) {
	
	TestRedis *tr = (TestRedis*)r;
	string *ca = NULL;
	string *cn = NULL;


	bool has = tr->readData(&ca, &cn);

	if (has && ca != NULL && cn != NULL)
	{
		CCLog("read from chatInfo");
		cha->assign(ca->c_str());
		con->assign(cn->c_str());
	}else {
		//CCLog("has ca cn %d %x %x", has, ca, cn);
	}

	delete ca;
	delete cn;
	return has;
}






void startReceiveRedis(int cid) {
	CCLog("startReceiveRedis %d", cid);
	if (receive != NULL)
	{
		((TestRedis*)receive)->stopReceiver();
		receive = NULL;
	}
	if (cid == -1)
	{
		return;
	}
	TestRedis *tr = new TestRedis();
	receive = tr;
	tr->setChannel(cid);
	tr->runSubscribe();
}

void startSendRedis(int cid) {
	CCLog("startSendReids %d", cid);

	if (sender != NULL)
	{
		pthread_mutex_lock(&sender_mutex);
		((TestRedis*)sender)->stopSend();
		sender = NULL;
		pthread_mutex_unlock(&sender_mutex);
	}

	if (cid == -1)
	{
		return;
	}
	TestRedis *tr = new TestRedis();
	tr->setChannel(cid);
	sender = tr;
	tr->initSendThread();

}









bool sendMsgC(const char *con, int mid, int msgId) {
	if (sender != NULL)
	{
		return ((TestRedis*)sender)->sendMsg(con, mid, msgId);
	}
	return false;
}

bool getLostTime(long long *lostTime) {
	return ((TestRedis*)receive)->getLTime(lostTime);
}

int getMsgState(int msgId) {
	int state = 0;
	pthread_mutex_lock(&sender_mutex);
	state = Logic::getInstance()->getMsgState(msgId);
	pthread_mutex_unlock(&sender_mutex);
	return state;
}



