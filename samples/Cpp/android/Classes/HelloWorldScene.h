#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "HttpModel.h"
#include "cocos-ext.h"

using namespace cocos2d::ui;

class HelloWorld : public cocos2d::CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    void record(CCObject *pSender);
    void stop(CCObject*);
    void play(CCObject*);
    void send(CCObject*);
    void receive(CCObject*);
    
    void http(CCObject*);
    void *testR;
    
    virtual void update(float);
    void onLeft(CCObject*, cocos2d::ui::TouchEventType type);
    void selectItem(CCObject *, ListViewEventType);
    
    void response(bool suc, std::string, void*);
    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);
};

#endif // __HELLOWORLD_SCENE_H__
