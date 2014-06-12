//
//  LoginView.h
//  TestCpp
//
//  Created by dozengame on 14-5-16.
//
//

#ifndef __TestCpp__LoginView__
#define __TestCpp__LoginView__

#include <iostream>
#include "cocos-ext.h"
#include "cocos2d.h"
using namespace cocos2d;
using namespace extension;
using namespace ui;
class LoginView : public CCLayer, public CCIMEDelegate {
public:
    virtual bool init();
    virtual void update(float);
    static CCScene *scene();
    
    CREATE_FUNC(LoginView);

    
    virtual void keyboardWillShow(CCIMEKeyboardNotificationInfo& info)   {
        //CC_UNUSED_PARAM(info);
        
        /*
        CCLog("begin info %f %f %f %f", info.begin.origin.x, info.begin.origin.y, info.begin.size.width, info.begin.size.height);
        CCLog("end info %f %f %f %f", info.end.origin.x, info.end.origin.y, info.end.size.width, info.end.size.height);
        CCLog("dura info %f", info.duration);
        */
        ksize = info.end.size;
        setSizeYet = true;
        
    }
    virtual void keyboardDidShow(CCIMEKeyboardNotificationInfo& info)    {CC_UNUSED_PARAM(info);}
    virtual void keyboardWillHide(CCIMEKeyboardNotificationInfo& info)   {CC_UNUSED_PARAM(info);}
    virtual void keyboardDidHide(CCIMEKeyboardNotificationInfo& info)    {CC_UNUSED_PARAM(info);}
    
private:
    void onLogin(CCObject*, TouchEventType);
    void onReg(CCObject*, TouchEventType);
    void onPhonenum(CCObject*, TextFiledEventType);
    void onPassword(CCObject*, TextFiledEventType);
    void loginOver(bool, std::string, void*);
    void onBackground(CCObject *, TouchEventType);
    
    Widget *w;
    CCSize ksize;
    bool setSizeYet;
    
    TextField *password;
    Button *login;
    Button *reg;
    TextField *phoneNum;
    Label *error;
    
    bool checkYet;
    bool inLogin;
};

#endif /* defined(__TestCpp__LoginView__) */
