//
//  RegisterView.h
//  TestCpp
//
//  Created by dozengame on 14-5-16.
//
//

#ifndef __TestCpp__RegisterView__
#define __TestCpp__RegisterView__

#include <iostream>

#include "cocos-ext.h"
#include "cocos2d.h"
using namespace cocos2d;
using namespace extension;
using namespace ui;


class RegisterView : public CCLayer, public CCKeypadDelegate {
public:
    virtual bool init();
    virtual void update(float);
    static CCScene *scene();
    
    CREATE_FUNC(RegisterView);

    virtual void keyBackClicked();
    
private:
    void onReg(CCObject *, TouchEventType);
    void registerOver(bool suc, std::string s, void *param);
    void onBackground(CCObject*, TouchEventType);
    
    bool inReg;
    TextField *phonenum;
    TextField *password;
    TextField *password1;
    Label *error;
    
    
    Widget *w;
    
    
    
};

#endif /* defined(__TestCpp__RegisterView__) */
