//
//  LoginScene.cpp
//  BallParty
//
//  Created by dozengame on 14-5-13.
//
//

#include "LoginScene.h"
//#include "md5.h"
#include "Md5.h"
#include "Logic.h"
#include "RegistScene.h"

#include "WorldCup.h"
#include "ServiceCenter.h"


UIWidget* loginWidget;

CCScene* LoginScene::scene()
{
	CCScene* scene = NULL;
	do{
		scene = CCScene::create();
		CC_BREAK_IF(! scene);
		LoginScene* layer = LoginScene::create();
		CC_BREAK_IF(! layer);
		scene->addChild(layer);
	} while (0);
	return scene;
}
bool LoginScene::init(){
    CCSpriteFrameCache* pCache = CCSpriteFrameCache::sharedSpriteFrameCache();
    pCache->addSpriteFramesWithFile("Scene/LoginLayer/LoginSence0.plist");
    
    return true;
}
void LoginScene::onEnter(){
    CCLayer::onEnter();
	TouchGroup* ui = TouchGroup::create();
	loginWidget = GUIReader::shareReader()->widgetFromJsonFile("Scene/LoginSence/LoginSence.json");
    loginWidget->setSizeType(SIZE_ABSOLUTE);
    loginWidget->setSize(CCDirector::sharedDirector()->getVisibleSize());
    
    ui->addWidget(loginWidget);
	ui->setAnchorPoint(CCPointZero);
	this->addChild(ui);
    
    
    //登陆区域
    UIPanel* login_panel = (UIPanel*)loginWidget->getChildByTag(4);
    UIPanel* bg_panel = (UIPanel*)loginWidget->getChildByTag(63);

    
    //用户名
    m_userName = CCEditBox::create(CCSize(320,50), CCScale9Sprite::createWithSpriteFrameName("button.png"));
	m_userName->setFontSize(23);
	m_userName->setAnchorPoint(CCPointZero);
	m_userName->setPlaceHolder("请输入用户名");
    m_userName->setPlaceholderFontColor(ccc3(248,252,255));
	m_userName->setInputMode(kEditBoxInputModeAny);
	m_userName->setReturnType(kKeyboardReturnTypeDone);
	m_userName->setPosition(ccp(153,378));
	m_userName->setDelegate(this);
	m_userName->setTouchPriority(0);
    //密码
    m_passWord = CCEditBox::create(CCSize(320,50), CCScale9Sprite::createWithSpriteFrameName("button.png"));
	m_passWord->setFontSize(23);
	m_passWord->setAnchorPoint(CCPointZero);
	m_passWord->setPlaceHolder("请输入密码");
    m_passWord->setPlaceholderFontColor(ccc3(248,252,255));
	m_passWord->setInputMode(kEditBoxInputModeAny);
    m_passWord->setInputFlag(kEditBoxInputFlagPassword);
	m_passWord->setReturnType(kKeyboardReturnTypeDone);
	m_passWord->setPosition(ccp(153,310));
	m_passWord->setDelegate(this);
	m_passWord->setTouchPriority(0);

    UIButton* loginBtn = (UIButton*)login_panel->getChildByTag(8);
    loginBtn->addTouchEventListener(this,toucheventselector(LoginScene::loginPress));
    
    UIButton* registerBtn = (UIButton*)login_panel->getChildByTag(44);
    registerBtn->addTouchEventListener(this, toucheventselector(LoginScene::registerPress));
    
    //切换账号
    UIButton* tabLoginBtn = (UIButton*)login_panel->getChildByTag(87);
    tabLoginBtn->addTouchEventListener(this, toucheventselector(LoginScene::tabloginPress));
    
    login_panel->addNode(m_userName);
    login_panel->addNode(m_passWord);
    
    //更新显示
    refurbishScene();

    
}

void LoginScene::refurbishScene(){
    //本地有存储用户名
    string loginNmaeKey = CCUserDefault::sharedUserDefault()->getStringForKey("loginName");
    
    int avatarKey = CCUserDefault::sharedUserDefault()->getIntegerForKey("avatar");
    CCLog("avatarKey:%d", avatarKey);
    
    UIPanel* login_panel = (UIPanel*)loginWidget->getChildByTag(4);
    UIImageView* userIcon = (UIImageView*)login_panel->getChildByName("userIcon");
    UIImageView* usernameLable = (UIImageView*)login_panel->getChildByName("usernameLiImage");
    
    UIButton* tabLoginBtn = (UIButton*)login_panel->getChildByTag(87);
    
    //输入框位置
    UIImageView* passwordLiImage = (UIImageView*)login_panel->getChildByTag(57);
    UIImageView* usernameLiImage = (UIImageView*)login_panel->getChildByTag(60);
    CCPoint usernamePoint = usernameLiImage->getPosition();
    m_userName->setPosition(ccp(usernamePoint.x - 407/2 + 40, usernamePoint.y - 15));
    CCPoint pwdPoint = passwordLiImage->getPosition();
    m_passWord->setPosition(ccp(pwdPoint.x - 407/2 + 40, pwdPoint.y - 15));
    
    if(avatarKey > 0 && avatarKey <= 32){
        char avatarItem[50];
        sprintf(avatarItem, "flags/%d.png", avatarKey);
        userIcon->loadTexture(avatarItem);
    }

    
    
    if (loginNmaeKey != "") {
        //隐藏用户输入框
        m_userName->setText(loginNmaeKey.c_str());
        m_userName->setVisible(false);
        
        usernameLable->setVisible(false);
        
        UILabel* iconNameLabel = (UILabel*)userIcon->getChildByTag(47);
        iconNameLabel->setText(loginNmaeKey.c_str());
        
        tabLoginBtn->setEnabled(true);
    }else{
        m_userName->setVisible(true);
        usernameLable->setVisible(true);
        userIcon->setVisible(false);
        
        tabLoginBtn->setEnabled(false);
    }
}

void LoginScene::loginEnd(bool suc, std::string s, void*param)
{
    if(suc == false){
        return;
    }
    
	UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
    
    NetLogin loginData = pUserService->analyzeLoginRect(s);
    if(loginData.status == 1){
    //登陆成功
        //保存本地用户名
        CCUserDefault::sharedUserDefault()->setStringForKey("loginName", m_userName->getText());
        CCUserDefault::sharedUserDefault()->setStringForKey("passWord", md5(m_passWord->getText()));
        CCUserDefault::sharedUserDefault()->flush();
        
        
        //Logic::getInstance()->setUID(loginData.userid);
        pUserService->getUser()->uid = loginData.userid;
        //初始化登录信息
        CCDirector* pDirector = CCDirector::sharedDirector();
        if(loginData.isPerfect == 1){
            CCLog("loginEnd finish");
            CCScene* pScene = WorldCup::scene();
            //pDirector->replaceScene(pScene);
            CCScene *ns = CCTransitionSlideInR::create(transTime, pScene);
            
            pDirector->replaceScene(ns);
            
        }else{
            CCLog("Userid::%d",loginData.userid);
            CCUserDefault::sharedUserDefault()->setIntegerForKey("setPerfectFormId", loginData.userid);
            CCUserDefault::sharedUserDefault()->flush();
            
            CCScene* pScene = RegistScene::scene();
            
            pDirector->replaceScene(getTransScene(pScene));
            
        }
        
        
    }else{
    //登陆失败
        CCLog("Login fail!");
        CCMessageBox("账号或密码错误！", "提示");
        //删除本地用户名
        CCUserDefault::sharedUserDefault()->setStringForKey("loginName", "");
        CCUserDefault::sharedUserDefault()->flush();
        
        //Logic::getInstance()->setLoginName("");
        
        refurbishScene();
    }
}

void LoginScene::registerPress(CCObject *pSender,TouchEventType type)
{
	if(type == TOUCH_EVENT_ENDED){
        
        CCDirector* pDirector = CCDirector::sharedDirector();
        
        //进入注册页面
        CCScene* pScene = RegistScene::scene();
        pDirector->replaceScene(pScene);

	}
    
}
//切换账号
void LoginScene::tabloginPress(CCObject *pSender,TouchEventType type)
{
    if (type == TOUCH_EVENT_ENDED){
        //删除本地用户名
        CCUserDefault::sharedUserDefault()->setStringForKey("loginName", "");
        CCUserDefault::sharedUserDefault()->flush();
        
        refurbishScene();
    }
}
//登录
void LoginScene::loginPress(CCObject *pSender,TouchEventType type)
{
	switch (type)
	{
        case TOUCH_EVENT_BEGAN:
            break;
        case TOUCH_EVENT_ENDED:
        {
            //直接发送 暂时忽略验证

            UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
            pUserService->login(m_userName->getText(),md5(m_passWord->getText()),this,MyHttpResp(&LoginScene::loginEnd));
        }
            break;
        case TOUCH_EVENT_CANCELED:
        {
            
        }
            break;
        case TOUCH_EVENT_MOVED:
        {
            
        }
            break;
        default:
            break;
	}

}
void LoginScene::editBoxEditingDidBegin( cocos2d::extension::CCEditBox* editBox )
{
}

void LoginScene::editBoxEditingDidEnd( cocos2d::extension::CCEditBox* editBox )
{
    
}

void LoginScene::editBoxTextChanged( cocos2d::extension::CCEditBox* editBox, const std::string& text )
{
    
}

void LoginScene::editBoxReturn( cocos2d::extension::CCEditBox* editBox )
{
    
}

void LoginScene::onExit(){
    
    

}