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
#include "GloabData.h"
#include "ChatRoomScene.h"
#include "md5.h"

#include <string>

using namespace cocos2d;
using namespace gui;
using namespace extension;

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
private:

    //注册到第几项
    int m_currsetup;
    //保存值
    string v_phoneNum;
    string v_password;
    string v_cfmPassword;
    string v_itrsPhoneNum;
    int v_userid;
    
    //球队信息
    map<int, itemstruct> ballItem;
    
    //电话号码
    CCEditBox* m_phoneNum;
    //真实姓名
    CCEditBox* m_realName;
    //密码
    CCEditBox* m_password;
    //确认密码
    CCEditBox* m_cfmPassWord;
    //推荐人号码
    CCEditBox* m_itrsPhoneNum;
    //地区
    CCEditBox* m_areaBox;
    
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
    void comebackPress(CCObject *pSender,TouchEventType type);
    void sexCheckBoxPress(CCObject* pSender, CheckBoxEventType type);
    
    void registEnd(bool suc, std::string s, void*param);
    void setPerfectEnd(bool suc, std::string s, void*param);
    
    void setPanelTitle(const char* title);
    
    void showPanel(int setup);
    
};
#endif /* defined(__BallParty__RegistScene__) */
