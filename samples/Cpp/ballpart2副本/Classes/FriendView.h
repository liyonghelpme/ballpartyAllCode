//
//  FriendView.h
//  TestCpp
//
//  Created by dozengame on 14-5-19.
//
//

#ifndef __TestCpp__FriendView__
#define __TestCpp__FriendView__

#include <iostream>

#include "cocos2d.h"
#include "cocos-ext.h"


using namespace cocos2d;
using namespace extension;
using namespace ui;

class FriendView : public CCLayer{
public:
    virtual bool init();
    virtual void update(float);
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    CREATE_FUNC(FriendView);
private:
    void onBack(CCObject *, TouchEventType);
    
    
    cocos2d::ui::Widget *w;
    
    ListView *lv;
    Layout *alpha;
    Layout *people;
};

#endif /* defined(__TestCpp__FriendView__) */
