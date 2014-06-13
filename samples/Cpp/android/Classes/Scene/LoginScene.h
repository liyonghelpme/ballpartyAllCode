//
//  LoginScene.h
//  BallParty
//
//  Created by dozengame on 14-5-13.
//
//

#ifndef __BallParty__LoginScene__
#define __BallParty__LoginScene__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "UserService.h"

//#include "RegistScene.h"
//#include "ChatRoomScene.h"


using namespace cocos2d;
using namespace ui;
using namespace extension;

class LoginScene:public CCLayer,public CCEditBoxDelegate
{
public:
    LoginScene(){};
    ~LoginScene(){};
    CREATE_FUNC(LoginScene);
    static CCScene* scene();
    bool init();
    void onEnter();
    void onExit();
    void loginPress(CCObject *pSender,TouchEventType type);
    void registerPress(CCObject *pSender,TouchEventType type);
    void tabloginPress(CCObject *pSender,TouchEventType type);
    void loginEnd(bool suc, std::string s, void*param);
    void refurbishScene();
    virtual void update(float);

private:
    UIPanel *midView;
    void checkKeyBoard();
    void pinyinMove();

    CCSize ksize;

    //用户名
    //CCEditBox* m_userName;
    //密码
    //CCEditBox* m_passWord;

    UITextField *username;
    UITextField *password;

    
    //开始进入编辑
	virtual void editBoxEditingDidBegin(cocos2d::extension::CCEditBox* editBox);
	//结束编辑
	virtual void editBoxEditingDidEnd(cocos2d::extension::CCEditBox* editBox);
	//编辑框文本改变
	virtual void editBoxTextChanged(cocos2d::extension::CCEditBox* editBox, const std::string& text);
	//当触发return后的回调函数
	virtual void editBoxReturn(cocos2d::extension::CCEditBox* editBox);
};
#endif /* defined(__BallParty__LoginScene__) */
