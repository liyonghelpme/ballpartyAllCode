//
//  FinishReg.h
//  TestCpp
//
//  Created by dozengame on 14-5-19.
//
//

#ifndef __TestCpp__FinishReg__
#define __TestCpp__FinishReg__

#include <iostream>

#include "cocos-ext.h"
#include "cocos2d.h"
using namespace cocos2d;
using namespace extension;
using namespace ui;
class FinishReg : public CCLayer {
public:
    virtual bool init();
    virtual void update(float);
    static CCScene *scene();
    
    CREATE_FUNC(FinishReg);
private:
    void onFin(CCObject *, TouchEventType);
    void registerOver(bool suc, std::string s, void *param);
    void onHead(CCObject*, TouchEventType);
    
    bool inReg;
    
    TextField *name;
    Button *finBut;
    
    Label *error;
    Layout *netLay;
    Button *head;
    
    
    Widget *w;
    
    bool setHeadYet;
    
    
};

#endif /* defined(__TestCpp__FinishReg__) */
