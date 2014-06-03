//
//  ChatInfo.h
//  TestCpp
//
//  Created by dozengame on 14-5-19.
//
//

#ifndef __TestCpp__ChatInfo__
#define __TestCpp__ChatInfo__

#include <iostream>

#include "cocos-ext.h"
#include "cocos2d.h"
using namespace cocos2d;
using namespace extension;
using namespace ui;
class ChatInfo : public CCLayer, public CCIMEDelegate {
public:
    virtual bool init();
    virtual void update(float);
    static cocos2d::CCScene* scene();
    CREATE_FUNC(ChatInfo);

private:
    void onPeople(CCObject*, TouchEventType);
    
    void onBack(CCObject *obj, TouchEventType tt);
    
    Widget *w;
    
    Layout *row;
    
    ListView *lv;
    
    bool getYet;
    
};

#endif /* defined(__TestCpp__ChatInfo__) */
