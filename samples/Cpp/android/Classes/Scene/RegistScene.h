//
//  RegistScene.h
//  BallParty
//
//  Created by dozengame on 14-5-13.
//
//

#ifndef __BallParty__RegistScene__
#define __BallParty__RegistScene__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "UserService.h"
//#include "GloabData.h"
//#include "ChatRoomScene.h"
//#include "md5.h"

#include <string>
#include <ctype.h>
#include "regex.h"

using namespace cocos2d;
using namespace ui;
using namespace extension;

using namespace std;


//球队信息
typedef struct itemstruct_DDK{
    int num;
    string name;
    string iconPath;
}itemstruct;


class RegistScene:public CCLayer,public CCEditBoxDelegate
{
public:
    RegistScene(){};
    ~RegistScene(){};
    CREATE_FUNC(RegistScene);
    static CCScene* scene();
    bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void update(float diff);

    virtual void keyBackClicked();
private:
    CCSize ksize;
    UIPanel *rootPerfect;

    
    void checkKeyBoard();
    void pinyinMove();

    UIPanel *midView;

    int v_userid;
    map<int, itemstruct> ballItem;
    
    //CCEditBox *m_areaBox;
    UITextField *area;

    //注册到第几项
    int m_currsetup;
    //保存值
    string v_phoneNum;
    string v_password;
    string v_cfmPassword;
    string v_itrsPhoneNum;
    string v_verityNum;
    int v_refereeId;
    
    //电话号码
    //CCEditBox* m_phoneNum;
    UITextField *myPhoneNum;

    //真实姓名
    //CCEditBox* m_realName;
    UITextField *nickname;

    //密码
    //CCEditBox* m_password;
    UITextField *password;


    //确认密码
    CCEditBox* m_cfmPassWord;
    //推荐人号码
    
    //CCEditBox* m_itrsPhoneNum;
    UITextField *refPhoneNum;

    //验证码
    //CCEditBox* m_verityNum;
    UITextField *verifyCode;


    //开始进入编辑
	virtual void editBoxEditingDidBegin(cocos2d::extension::CCEditBox* editBox);
	//结束编辑
	virtual void editBoxEditingDidEnd(cocos2d::extension::CCEditBox* editBox);
	//编辑框文本改变
	virtual void editBoxTextChanged(cocos2d::extension::CCEditBox* editBox, const std::string& text);
	//当触发return后的回调函数
	virtual void editBoxReturn(cocos2d::extension::CCEditBox* editBox);
    
    void registPress(CCObject *pSender,TouchEventType type);
    void finishPress(CCObject *pSender,TouchEventType type); 
    void listviewPress(CCObject *pSender,TouchEventType type);
    void worldCupStrongPress(CCObject *pSender,TouchEventType type);
    void refereePress(CCObject *pSender,TouchEventType type);
    void vphonePress(CCObject *pSender,TouchEventType type);
    void verifyPress(CCObject *pSender,TouchEventType type);
    void resendCodePress(CCObject *pSender,TouchEventType type);
    
    void registEnd(bool suc, std::string s, void*param);
    void setPerfectEnd(bool suc, std::string s, void*param);
    void refereeEnd(bool suc, std::string s, void*param);
    void vphoneEnd(bool suc, std::string s, void*param);
    void vResendPhoneEnd(bool suc, std::string s, void*param);
    void verifyEnd(bool suc, std::string s, void*param);
    
    void setPanelTitle(const char* title);
    
    void showPanel(int setup);
    
    void comebackPress(CCObject *pSender,TouchEventType type);
    void sexCheckBoxPress(CCObject* pSender, CheckBoxEventType type);
    
    void cfmOkPress(CCObject* pSender, TouchEventType type);
    void cfmCancelPress(CCObject* pSender, TouchEventType type);
    void provisionPress(CCObject* pSender, TouchEventType type);
    void cfmBgPress(CCObject* pSender, TouchEventType type);
    
    };
#endif /* defined(__BallParty__RegistScene__) */
