//
//  RegistScene.cpp
//  BallParty
//
//  Created by dozengame on 14-5-13.
//
//

#include "RegistScene.h"
#include "LoginScene.h"
#include "CoverView.h"


UIButton* m_registBtn;
UIWidget* addshow;
CoverView* coverView;

CCScene* RegistScene::scene()
{
	CCScene* scene = NULL;
	do{
		scene = CCScene::create();
		CC_BREAK_IF(! scene);
		RegistScene* layer = RegistScene::create();
		CC_BREAK_IF(! layer);
		scene->addChild(layer);
	} while (0);
	return scene;
}
bool RegistScene::init(){
    CCLayer::init();
    
    v_phoneNum = "";
    v_password = "";
    v_itrsPhoneNum = "";
    v_userid = 0;
    
    return true;
}
void RegistScene::onEnter(){
    CCLayer::onEnter();
     
	TouchGroup* ui = TouchGroup::create();
	addshow = GUIReader::shareReader()->widgetFromJsonFile("Scene/RegistScene/RegistScene.json");
    addshow->setSizeType(SIZE_ABSOLUTE);
    addshow->setSize(CCDirector::sharedDirector()->getVisibleSize());
	ui->addWidget(addshow);
	ui->setAnchorPoint(CCPointZero);
	this->addChild(ui);
    
    
    //coverView
    coverView = CoverView::create(CCRect(70,720,500,180), CCSize(180, 180), 180, 0.3f);
    coverView->setPosition(70,720);

    for(int i=1; i<32;i++){
        char iconPath[100];
        sprintf(iconPath, "flag/%d.png", i);
        CCSprite* player = CCSprite::create(iconPath);
        
        /*
        UILabel* text = UILabel::create();
        text->setText("test");
        text->setColor(ccc3(0,0,0));
        text->setPosition(ccp(50,50));
        player->addChild(text);
        */
        
        coverView->addCard(player);
    }
    coverView->setOffsetPosition(ccp(50, 50));
    
    coverView->adjusetEndScrollView();
    
    //可视区域
    UIPanel* setup1 = (UIPanel*)addshow->getChildByTag(20);
    UIPanel* setup2 = (UIPanel*)addshow->getChildByTag(44);
    
    //add coverView
    setup2->addNode(coverView);
    
    //32强
    UILabel* likeLabel = (UILabel*)addshow->getChildByTag(44)->getChildByTag(89)->getChildByTag(146);
    likeLabel->addTouchEventListener(this, toucheventselector(RegistScene::worldCupStrongPress));
    
    UIPanel* listPanel = (UIPanel*)addshow->getChildByTag(102);
    UIListView* listView = (UIListView*)listPanel->getChildByTag(110);
    //listPanel->setEnabled(false);
    Layout* default_item = Layout::create();
    default_item->setTouchEnabled(true);
    //default_item->setSize(CCSizeMake(640, 30*75));
    
    listView->setItemModel(default_item);
    
    for(int i=0; i<30; i++){
        UIButton* m_option = UIButton::create();
        m_option->setTouchEnabled(true);
        m_option->loadTextures("Scene/RegistScene/bottom_line.png", "Scene/RegistScene/bottom_line.png", "Scene/RegistScene/bottom_line.png");
        //default_item->addChild(m_option);
        UILabel* m_label = UILabel::create();
        if(i % 2 == 0){
            m_label->setText("中华人民共和国");
        }else{
            m_label->setText("中国");
        }
        
        m_label->setFontSize(26);
        m_label->setColor(ccc3(0,0,0));
        m_label->setAnchorPoint(ccp(0, 0.5));
        m_label->setPosition(ccp(-310,0));
        m_option->addChild(m_label);
        
        char buffer[10];
        sprintf(buffer, "%d", i);
        m_option->setName(buffer);
        
        m_option->addTouchEventListener(this, toucheventselector(RegistScene::listviewPress));
        
        listView->pushBackCustomItem(m_option);
    }
    
    setup2->setEnabled(false);
    
    //注册按钮
    m_registBtn = (UIButton*)addshow->getChildByTag(20)->getChildByTag(14);
    m_registBtn->addTouchEventListener(this,toucheventselector(RegistScene::registPress));
    //完成注册按钮
    UIButton* m_finishBtn = (UIButton*)addshow->getChildByTag(44)->getChildByTag(94);
    m_finishBtn->addTouchEventListener(this, toucheventselector(RegistScene::finishPress));
    //返回按钮
    UIButton* m_comebackBtn = (UIButton*)addshow->getChildByTag(20)->getChildByTag(29)->getChildByTag(154);
    m_comebackBtn->addTouchEventListener(this, toucheventselector(RegistScene::comebackPress));
    
    //性别复选框
    UICheckBox* manCheckBox = (UICheckBox*)addshow->getChildByTag(44)->getChildByTag(224);
    UICheckBox* WomCheckBox = (UICheckBox*)addshow->getChildByTag(44)->getChildByTag(227);
    manCheckBox->addEventListenerCheckBox(this, checkboxselectedeventselector(RegistScene::sexCheckBoxPress));
    WomCheckBox->addEventListenerCheckBox(this, checkboxselectedeventselector(RegistScene::sexCheckBoxPress));
    manCheckBox->setSelectedState(true);
    
    //手机号码
    m_phoneNum = CCEditBox::create(CCSize(300,40), CCScale9Sprite::createWithSpriteFrameName("button.png"));
	m_phoneNum->setFontSize(23);
	m_phoneNum->setAnchorPoint(CCPointZero);
	m_phoneNum->setPlaceHolder("请输入手机号");
	m_phoneNum->setInputMode(kEditBoxInputModePhoneNumber);
	m_phoneNum->setReturnType(kKeyboardReturnTypeDone);
    m_phoneNum->setFontColor(ccc3(80,80,80));
	m_phoneNum->setPosition(ccp(180,767));
	m_phoneNum->setDelegate(this);
	m_phoneNum->setTouchPriority(0);

    //密码
    m_password = CCEditBox::create(CCSize(300,40),CCScale9Sprite::createWithSpriteFrameName("button.png"));
	m_password->setFontSize(23);
	m_password->setAnchorPoint(CCPointZero);
	m_password->setPlaceHolder("请输入密码");
	m_password->setInputMode(kEditBoxInputModeAny);
	m_password->setInputFlag(kEditBoxInputFlagPassword);
	m_password->setReturnType(kKeyboardReturnTypeDone);
    m_password->setFontColor(ccc3(80,80,80));
	m_password->setPosition(ccp(180,663));
	m_password->setDelegate(this);
	m_password->setTouchPriority(0);
    
    //确认密码
    m_cfmPassWord = CCEditBox::create(CCSize(300,40),CCScale9Sprite::createWithSpriteFrameName("button.png"));
	m_cfmPassWord->setFontSize(23);
	m_cfmPassWord->setAnchorPoint(CCPointZero);
	m_cfmPassWord->setPlaceHolder("请确认密码");
	m_cfmPassWord->setInputMode(kEditBoxInputModeAny);
	m_cfmPassWord->setInputFlag(kEditBoxInputFlagPassword);
	m_cfmPassWord->setReturnType(kKeyboardReturnTypeDone);
    m_cfmPassWord->setFontColor(ccc3(80,80,80));
	m_cfmPassWord->setPosition(ccp(180,561));
	m_cfmPassWord->setDelegate(this);
	m_cfmPassWord->setTouchPriority(0);

     //真实姓名
     m_realName = CCEditBox::create(CCSize(300,40),CCScale9Sprite::createWithSpriteFrameName("button.png"));
     m_realName->setFontSize(23);
     m_realName->setAnchorPoint(CCPointZero);
     m_realName->setPlaceHolder("请输入姓名");
     m_realName->setInputMode(kEditBoxInputModeAny);
     m_realName->setReturnType(kKeyboardReturnTypeDone);
     m_realName->setFontColor(ccc3(80,80,80));
     m_realName->setPosition(ccp(180,522));
     m_realName->setDelegate(this);
     m_realName->setTouchPriority(0);
    
    //推荐人手机号
    m_itrsPhoneNum = CCEditBox::create(CCSize(300,40),CCScale9Sprite::createWithSpriteFrameName("button.png"));
	m_itrsPhoneNum->setFontSize(23);
	m_itrsPhoneNum->setAnchorPoint(CCPointZero);
	m_itrsPhoneNum->setPlaceHolder("请输入推荐人手机号");
	m_itrsPhoneNum->setInputMode(kEditBoxInputModePhoneNumber);
	m_itrsPhoneNum->setReturnType(kKeyboardReturnTypeDone);
    m_itrsPhoneNum->setFontColor(ccc3(80,80,80));
	m_itrsPhoneNum->setPosition(ccp(180,458));
	m_itrsPhoneNum->setDelegate(this);
	m_itrsPhoneNum->setTouchPriority(0);

    //地区
    m_areaBox = CCEditBox::create(CCSize(300,40),CCScale9Sprite::createWithSpriteFrameName("button.png"));
    m_areaBox->setFontSize(23);
    m_areaBox->setAnchorPoint(CCPointZero);
    m_areaBox->setPlaceHolder("请输入地区");
    m_areaBox->setInputMode(kEditBoxInputModeAny);
    m_areaBox->setReturnType(kKeyboardReturnTypeDone);
    m_areaBox->setFontColor(ccc3(80,80,80));
    m_areaBox->setPosition(ccp(180,366));
    m_areaBox->setDelegate(this);
    m_areaBox->setTouchPriority(0);
    
    setup1->addNode(m_phoneNum);
    setup1->addNode(m_password);
    setup1->addNode(m_cfmPassWord);
    setup1->addNode(m_itrsPhoneNum);
    
    setup2->addNode(m_realName);
    setup2->addNode(m_areaBox);
    m_realName->setEnabled(false);
    m_areaBox->setEnabled(false);
    //showPanel(2);
    listPanel->setEnabled(false);
    
    //设置EditBox位置
    ImageView* phoneLiImage = (ImageView*)setup1->getChildByTag(167);
    ImageView* passwordLiImage = (ImageView*)setup1->getChildByTag(172);
    ImageView* vpasswordLiImage = (ImageView*)setup1->getChildByTag(174);
    ImageView* refereeLiImage = (ImageView*)setup1->getChildByTag(201);
    ImageView* usernameLiImage = (ImageView*)setup2->getChildByTag(191);
    ImageView* areaLiImage = (ImageView*)setup2->getChildByTag(193);
    CCPoint phonePoint = phoneLiImage->getPosition();
    CCPoint passwordPoint = passwordLiImage->getPosition();
    CCPoint vpasswordPoint = vpasswordLiImage->getPosition();
    CCPoint refereePoint = refereeLiImage->getPosition();
    CCPoint usernamePoint = usernameLiImage->getPosition();
    CCPoint areaPoint = areaLiImage->getPosition();
    m_phoneNum->setPosition(ccp(phonePoint.x - 190, phonePoint.y - 5));
    m_password->setPosition(ccp(passwordPoint.x -190, passwordPoint.y -5));
    m_cfmPassWord->setPosition(ccp(vpasswordPoint.x-190, vpasswordPoint.y-5));
    m_itrsPhoneNum->setPosition(ccp(refereePoint.x-190, refereePoint.y-5));
    m_realName->setPosition(ccp(usernamePoint.x-190, usernamePoint.y-5));
    m_areaBox->setPosition(ccp(areaPoint.x-190, areaPoint.y-5));
    
    
    
    //如果只是资料未填写跳到完善资料
    int setPerfectFormId = CCUserDefault::sharedUserDefault()->getIntegerForKey("setPerfectFormId");
    CCLog("setPerfectFormId::%d",setPerfectFormId);
    if( setPerfectFormId > 0){
        CCUserDefault::sharedUserDefault()->setIntegerForKey("setPerfectFormId", 0);
        CCUserDefault::sharedUserDefault()->flush();
        
        v_userid = setPerfectFormId;
        string tmp_loginName = CCUserDefault::sharedUserDefault()->getStringForKey("loginName");
        //账号label
        UILabel* usernameLabel = (UILabel*)addshow->getChildByTag(44)->getChildByTag(179)->getChildByTag(187);
        usernameLabel->setText(tmp_loginName);
        showPanel(2);
    }
    
    
}


//注册
void RegistScene::registPress( CCObject *pSender,TouchEventType type )
{
    
	if (type == TOUCH_EVENT_ENDED)
	{


        v_phoneNum = m_phoneNum->getText();
        v_password = m_password->getText();
        v_cfmPassword = m_cfmPassWord->getText();
        v_itrsPhoneNum = m_itrsPhoneNum->getText();
        
        UICheckBox* v_arrgeement = (UICheckBox*)addshow->getChildByTag(20)->getChildByTag(45);
        
        //选项不为空
        if(v_password=="" || v_phoneNum=="" || v_itrsPhoneNum == ""){
            CCMessageBox("所有选项不能为空！", "提示");
            return;
        }
        
        //确认密码是否一致
        if ( strcmp(v_password.c_str(), v_cfmPassword.c_str()) != 0) {
            CCMessageBox("密码不一致！", "提示");
            return;
        }
        
        if( !v_arrgeement->getSelectedState() ){
            
            CCMessageBox("请阅读并同意使用条款和隐私政权！", "提示");
            return;
        }
        
        UserService::shareUserService()->registe(v_phoneNum, MD5(v_password).toString(), MD5(v_cfmPassword).toString(), v_itrsPhoneNum,this,MyHttpResp(&RegistScene::registEnd));
        CCLOG("finish");
        
	}
}

//comeback
void RegistScene::comebackPress( CCObject *pSender,TouchEventType type )
{
    
	if (type == TOUCH_EVENT_ENDED)
	{
        CCDirector* pDirector = CCDirector::sharedDirector();
        CCScene* pScene = LoginScene::scene();
        pDirector->replaceScene(pScene);
        
        CCLog("back btn click!");
	}
}
//checkbox event
void RegistScene::sexCheckBoxPress(CCObject* pSender, CheckBoxEventType type){
    //性别复选框
    UICheckBox* manCheckBox = (UICheckBox*)addshow->getChildByTag(44)->getChildByTag(224);
    UICheckBox* WomCheckBox = (UICheckBox*)addshow->getChildByTag(44)->getChildByTag(227);
    
    UICheckBox* curCheckBox = (UICheckBox*)pSender;
    string curCheckName = curCheckBox->getName();
    if(curCheckName == "sexCheckBox1"){
        manCheckBox->setSelectedState(true);
        WomCheckBox->setSelectedState(false);
    }else{
        manCheckBox->setSelectedState(false);
        WomCheckBox->setSelectedState(true);
        
    }
    CCLog(curCheckBox->getName());
}

//finish
void RegistScene::finishPress( CCObject *pSender,TouchEventType type )
{
    
	if (type == TOUCH_EVENT_ENDED)
	{
        int likeNum = coverView->getCurCardIndex();
        char likeItem[50];
        sprintf(likeItem, "%d", likeNum + 1);
        char useridItem[50];
        sprintf(useridItem, "%d", v_userid);
        
        string  realName = m_realName->getText();
        string  areaBox = m_areaBox->getText();
        
        if(realName == "" || areaBox == ""){
            CCMessageBox("所有选项不能为空！", "提示");
            return;
        }
        
        string m_gender;
        UICheckBox* manCheckBox = (UICheckBox*)addshow->getChildByTag(44)->getChildByTag(224);
        if(manCheckBox->getSelectedState()){
            m_gender = "1";
        }else{
            m_gender = "0";
        }
        
        UserService::shareUserService()->setPerfect(useridItem,m_realName->getText(), m_gender, m_areaBox->getText(), likeItem, this, MyHttpResp(&RegistScene::setPerfectEnd));
        CCLog("set perfect");

	}
}

//listview press
void RegistScene::listviewPress( CCObject *pSender,TouchEventType type )
{
    UIButton* m_btn = dynamic_cast<UIButton*>(pSender);
    
	if (type == TOUCH_EVENT_ENDED)
	{
        CCMessageBox(m_btn->getName(), "提示");
        //CCLOG(m_btn->getName());
        UIPanel* listPanel = (UIPanel*)addshow->getChildByTag(102);
        listPanel->setEnabled(false);
        
        UILabel* likeLabel = (UILabel*)addshow->getChildByTag(44)->getChildByTag(89)->getChildByTag(146);
        likeLabel->setText(m_btn->getName());
	}
}

//32强
void RegistScene::worldCupStrongPress( CCObject *pSender,TouchEventType type )
{
    
	if (type == TOUCH_EVENT_ENDED)
	{
        UIPanel* listPanel = (UIPanel*)addshow->getChildByTag(102);
        listPanel->setEnabled(true);
	}
}

void RegistScene::setPerfectEnd(bool suc, std::string s, void*param){
    
    bool isPerfected = UserService::shareUserService()->analyzePerfectRect(s);
    if(!isPerfected){
        CCMessageBox("完成资料失败！", "提示");
        return;
    }
    
    CCMessageBox("恭喜您注册成功！", "提示");
    
    //完成暂时跳转到登陆界面
    //保存本地用户名
    CCUserDefault::sharedUserDefault()->setStringForKey("loginName", v_phoneNum);
    CCUserDefault::sharedUserDefault()->setIntegerForKey("avatar", coverView->getCurCardIndex() + 1);
    CCUserDefault::sharedUserDefault()->flush();
    CCDirector* pDirector = CCDirector::sharedDirector();
    CCScene* pScene = LoginScene::scene();
    pDirector->replaceScene(pScene);
    CCLog("setPerfectEnd!");
}

void RegistScene::registEnd(bool suc, std::string s, void*param)
{
    NetRegist regData = UserService::shareUserService()->analyzeRegistRect(s);
    CCLog("net regist %d", regData.status);
    if(regData.status == 1){
        v_userid = regData.userid;
        //注册成功进入到完善资料
        showPanel(2);
        //CCDirector* pDirector = CCDirector::sharedDirector();
        //CCScene* pScene = ChatRoomScene::scene();
        //pDirector->replaceScene(pScene);
    }else{
        CCMessageBox(regData.msg.c_str(), "提示");
    }
    //缓存到内存中
    //GloabData::shareGloabData()->self_User = user;
    
}

void RegistScene::showPanel(int setup){
    //可视区域
    UIPanel* setup1 = (UIPanel*)addshow->getChildByTag(20);
    UIPanel* setup2 = (UIPanel*)addshow->getChildByTag(44);
    
    if(setup == 2){
        //转到setup2
        m_phoneNum->setEnabled(false);
        m_password->setEnabled(false);
        m_cfmPassWord->setEnabled(false);
        m_itrsPhoneNum->setEnabled(false);
        setup1->setEnabled(false);
        
        setup2->setEnabled(true);
        m_realName->setEnabled(true);
        m_areaBox->setEnabled(true);
        
        //账号label
        UILabel* usernameLabel = (UILabel*)addshow->getChildByTag(44)->getChildByTag(179)->getChildByTag(187);
        usernameLabel->setText(v_phoneNum);

    }
    //CCFileUtils
    //CCDictionary
}

void RegistScene::editBoxEditingDidBegin( cocos2d::extension::CCEditBox* editBox )
{
}

void RegistScene::editBoxEditingDidEnd( cocos2d::extension::CCEditBox* editBox )
{
    
}

void RegistScene::editBoxTextChanged( cocos2d::extension::CCEditBox* editBox, const std::string& text )
{
    
}

void RegistScene::editBoxReturn( cocos2d::extension::CCEditBox* editBox )
{
    
}
void RegistScene::onExit(){
    CCLayer::onExit();
    
    
}