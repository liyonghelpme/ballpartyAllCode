//
//  LoadScene.h
//  ballparty
//
//  Created by dozengame on 14-5-29.
//
//

#ifndef __ballparty__LoadScene__
#define __ballparty__LoadScene__

#include <iostream>

#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace extension;
using namespace ui;
class LoadScene : public CCLayer{
public:
    virtual bool init();
    static cocos2d::CCScene* scene();
    CREATE_FUNC(LoadScene);
};

#endif /* defined(__ballparty__LoadScene__) */
