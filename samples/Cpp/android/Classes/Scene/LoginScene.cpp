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
    if(!CCLayer::init())
        return false;

    CCSpriteFrameCache* pCache = CCSpriteFrameCache::sharedSpriteFrameCache();
    pCache->addSpriteFramesWithFile("Scene/LoginLayer/LoginSence0.plist");
    scheduleUpdate();

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
    UIPanel* login_panel = (UIPanel*)UIHelper::seekWidgetByName(loginWidget, "bodyPanel");
    login_panel->setCloseIME(true);

    //loginWidget->getChildByTag(4);
    //UIPanel* bg_panel = (UIPanel*)loginWidget->getChildByTag(63);

    UIImageView *upImg = (UIImageView*)UIHelper::seekWidgetByName(loginWidget, "upImg");
    CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
    //float scale = fs.width/upImg->getSize().width;

    float scale = std::max(fs.width/upImg->getSize().width, fs.height/upImg->getSize().height);
    //if(scale > 1){
        upImg->setScale(scale);
    //}

    UIImageView *logo = (UIImageView*)UIHelper::seekWidgetByName(loginWidget, "logoImage");
    UIButton* loginBtn = (UIButton*)UIHelper::seekWidgetByName(loginWidget, "loginButton");
    //login_panel->getChildByTag(8);
    loginBtn->addTouchEventListener(this,toucheventselector(LoginScene::loginPress));
    

    float ms = std::min(fs.width/640, fs.height/960);
    if (ms < 1)
    {
        logo->setScale(ms);
        loginBtn->setScale(ms);
    }



    CCLog("init UIPanel");


    //用户名
    /*
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
    */

    username = (UITextField*)UIHelper::seekWidgetByName(loginWidget, "username");

    password = (UITextField*)UIHelper::seekWidgetByName(loginWidget, "password");
    midView = (UIPanel*)UIHelper::seekWidgetByName(loginWidget, "midView");

    CCLog("init midView");


    //密码
    /*
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
    */

    
    
    UIButton* registerBtn = (UIButton*)UIHelper::seekWidgetByName(loginWidget, "regButton");
    //login_panel->getChildByTag(44);
    registerBtn->addTouchEventListener(this, toucheventselector(LoginScene::registerPress));
    
    //切换账号
    UIButton* tabLoginBtn = (UIButton*)UIHelper::seekWidgetByName(loginWidget, "tabLoginButton");//login_panel->getChildByTag(87);
    tabLoginBtn->addTouchEventListener(this, toucheventselector(LoginScene::tabloginPress));
    
    //login_panel->addNode(m_userName);
    //login_panel->addNode(m_passWord);
    CCLog("init button");
    //更新显示
    refurbishScene();

    
}
void LoginScene::pinyinMove() {
    if (!isRunning()) {
        return;
    }
    //关闭输入框 
    if (ksize.height == 0)
    {
        midView->stopAllActions();
        midView->runAction(CCMoveTo::create(0.2, ccp(0, ksize.height)));  
        return;
    }
    
    UITextField *threeText[] = {
    username,
    password,
    NULL,   
    };
    bool findInput = false;
    //当前输入框和 键盘关联 移动这个输入框 Invisible 区域
    for(int i = 0; threeText[i] != NULL; i++){
        if (threeText[i]->getIsAttach())
        {
            CCLog("getAttachWithIME %d", i);
            CCPoint pos = threeText[i]->getPosition();
            float dy = ksize.height-pos.y;
            CCLog("positionY %f %f %f", pos.y, ksize.height, dy);

            //键盘遮挡住了 输入框下部 则 midView 向上移动
            //如果 没有 则 不用移动 例如负数？ 或者 
            if (dy > 0)
            {
                midView->stopAllActions();
                midView->runAction(CCMoveTo::create(0.2, ccp(0, dy+15)));  
            //移动到最下面即可
            }else {
                midView->stopAllActions();
                midView->runAction(CCMoveTo::create(0.2, ccp(0, 0)));  
            }
            findInput = true;
            break;
        }
    }
}
void LoginScene::checkKeyBoard() {
    bool km = CCUserDefault::sharedUserDefault()->getBoolForKey("keyboardMove");
    if(km) {

        CCUserDefault::sharedUserDefault()->setBoolForKey("keyboardMove", false);
        int kheight = CCUserDefault::sharedUserDefault()->getIntegerForKey("keyboardHeight");
        ksize.height = kheight;
        pinyinMove();
    }
}
void LoginScene::update(float diff){
    checkKeyBoard();
}
void LoginScene::refurbishScene(){
    //本地有存储用户名
    string loginNmaeKey = CCUserDefault::sharedUserDefault()->getStringForKey("loginName");
    
    int avatarKey = CCUserDefault::sharedUserDefault()->getIntegerForKey("avatar");
    CCLog("avatarKey:%d", avatarKey);
    
    //UIPanel* login_panel = (UIPanel*)loginWidget->getChildByTag(4);
    UIPanel* login_panel = (UIPanel*)UIHelper::seekWidgetByName(loginWidget, "bodyPanel");

    UIImageView* userIcon = (UIImageView*)UIHelper::seekWidgetByName(loginWidget, "userIcon");
    //login_panel->getChildByName("userIcon");
    UIImageView* usernameLable = (UIImageView*)UIHelper::seekWidgetByName(loginWidget, "usernameLiImage");
    
    //login_panel->getChildByName("usernameLiImage");
    
    UIButton* tabLoginBtn = (UIButton*)UIHelper::seekWidgetByName(loginWidget, "tabLoginButton");
    //login_panel->getChildByTag(87);
    
    //输入框位置
    UIImageView* passwordLiImage = (UIImageView*)UIHelper::seekWidgetByName(loginWidget, "passwordLiImage");

    //login_panel->getChildByTag(57);
    UIImageView* usernameLiImage = (UIImageView*)UIHelper::seekWidgetByName(loginWidget, "usernameLiImage");

    //login_panel->getChildByTag(60);
    CCPoint usernamePoint = usernameLiImage->getPosition();
    
    //m_userName->setPosition(ccp(usernamePoint.x - 407/2 + 40, usernamePoint.y - 15));
    
    CCPoint upos = username->getPosition();

    //username->setPosition(ccp(upos.x, usernamePoint.y - 15));


    CCPoint pwdPoint = passwordLiImage->getPosition();
    //m_passWord->setPosition(ccp(pwdPoint.x - 407/2 + 40, pwdPoint.y - 15));
    CCPoint ppos = password->getPosition();
    //password->setPosition(ccp(ppos.x, pwdPoint.y - 15));


    if(avatarKey > 0 && avatarKey <= 32){
        char avatarItem[50];
        sprintf(avatarItem, "flags/%d.png", avatarKey);
        userIcon->loadTexture(avatarItem);
    }

    
    //用户已经 登陆过了
    if (loginNmaeKey != "") {
        //隐藏用户输入框
        //m_userName->setText(loginNmaeKey.c_str());
        username->setText(loginNmaeKey.c_str());
        username->setEnabled(false);

        //m_userName->setVisible(false);
        
        usernameLable->setVisible(false);
        
        UILabel* iconNameLabel = (UILabel*)UIHelper::seekWidgetByName(loginWidget, "usernameLable");
        //userIcon->getChildByTag(47);
        iconNameLabel->setText(loginNmaeKey.c_str());
        
        tabLoginBtn->setEnabled(true);
    }else{
        //m_userName->setVisible(true);
        username->setEnabled(true);

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
        CCUserDefault::sharedUserDefault()->setStringForKey("loginName", username->getStringValue());
        CCUserDefault::sharedUserDefault()->setStringForKey("passWord", md5(password->getStringValue()));
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
            string uname = username->getStringValue();//m_userName->getText();

            string pass = password->getStringValue();//m_passWord->getText();
            if (uname.size() == 0 || pass.size() == 0)
            {
                CCMessageBox("账号或密码不能为空", "提示");
                return;
            }

            UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
            pUserService->login(uname,md5(pass),this,MyHttpResp(&LoginScene::loginEnd));
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