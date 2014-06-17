//
//  ConfigView.h
//  TestCpp
//
//  Created by dozengame on 14-5-19.
//
//

#ifndef __TestCpp__ConfigView__
#define __TestCpp__ConfigView__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"


using namespace cocos2d;
using namespace extension;
using namespace ui;

class ConfigView : public CCLayer{
public:
    virtual bool init();
    virtual void update(float);
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    CREATE_FUNC(ConfigView);

    virtual void keyBackClicked();
    
private:
    bool isOtherUser;
    
    void onBack(CCObject *, TouchEventType);
    void onQuit(CCObject *, TouchEventType);
    void initView();
    
    cocos2d::ui::Widget *w;
    
    bool getYet;
};


#endif /* defined(__TestCpp__ConfigView__) */
