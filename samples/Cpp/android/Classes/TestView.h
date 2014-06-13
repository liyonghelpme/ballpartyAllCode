//
//  TestView.h
//  ballparty
//
//  Created by dozengame on 14-5-22.
//
//

#ifndef __ballparty__TestView__
#define __ballparty__TestView__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace extension;
using namespace ui;
class TestView : public CCLayer{
public:
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    CREATE_FUNC(TestView);
    
};

#endif /* defined(__ballparty__TestView__) */
