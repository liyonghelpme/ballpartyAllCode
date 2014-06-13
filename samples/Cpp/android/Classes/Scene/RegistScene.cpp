//
//  RegistScene.cpp
//  BallParty
//
//  Created by dozengame on 14-5-13.
//
//

#include "RegistScene.h"
#include "Md5.h"
#include "WorldCup.h"
#include "Logic.h"
#include "LoginScene.h"
#include "CoverView.h"
#include "ServiceCenter.h"
#include "UserService.h"


UIButton* m_registBtn;
UIButton* m_reffereeBtn;
UIButton* m_vphoneBtn;
UIButton* m_verifyBtn;
UIWidget* addshow;
CoverView *coverView;

static char *teamNameId[] = {
    "阿尔及利亚",
    "阿根廷",
    "澳大利亚",
    "巴西",
    "比利时",
    "波黑",
    "德国",
    "俄罗斯",
    "厄瓜多尔",
    "法国",
    "哥伦比亚",
    "哥斯达黎加",
    "韩国",
    "荷兰",
    "洪都拉斯",
    "加纳",
    "喀麦隆",
    "科特迪瓦",
    "克罗地亚",
    "美国",
    "墨西哥",
    "尼日利亚",
    "葡萄牙",
    "日本",
    "瑞士",
    "乌拉圭",
    "西班牙",
    "希腊",
    "伊朗",
    "意大利",
    "英格兰",
    "智利",
};

string getTeamName(int id) {
    if (id > 0 && id <= 32) {
        string tc = teamNameId[id - 1];
        return tc;
    }else{
        return "";
    }
    
}

string trim(string& s){
    const string drop = " ";
    s.erase(s.find_last_not_of(drop) + 1);
    return s.erase(0, s.find_first_not_of(drop));
}

// 检验是否包含特殊字符
bool checkTextChineseOrNumberOrLetter(string str)
{
    int badNum = 0;
    
    int size = str.length();
    if (size <= 0)
        return false;
    
    char* pStr = new char[size];
    
    strcpy(pStr, str.c_str());
    for (int i = 0; i < size; i++)
    {
        if (!(pStr[i]>=0 && pStr[i]<=127))
            continue;
        if (ispunct(pStr[i]))
        {
            badNum ++;
        }
    }
    delete[] pStr;
    bool res = true;
    if (badNum > 0)
    {
        res = false;
    }
    return res;
}

bool checkTextString(string str){
    const char *cn = ".*(“|”|《|》|•|【|】|…).*";
    regmatch_t subs1[10];
    regex_t stuRT1;
    regcomp(&stuRT1, cn, REG_EXTENDED);
    int err1 = regexec(&stuRT1, str.c_str(), (size_t)10, subs1, 0);
    if (!err1) {
        regfree(&stuRT1);
        return false;
    }
    
    
    const char *eset = "^[A-Za-z0-9\u4e00-\u9fa5]+$";
    //regex_t re;
    regmatch_t   subs[10];
    regex_t stuRT;
    
    regcomp(&stuRT, eset, REG_EXTENDED);
    int err = regexec(&stuRT, str.c_str(), (size_t)10, subs, 0);
    
    if (err) {
        
        regfree(&stuRT);
        return false;
    }
    else
    {
        regfree(&stuRT);
        return true;
    }
}

bool delSpecialChar(string str)
{
    string m_str = str;
    //string m_str = "asdf=";
    CCLog("delSpecialChar");
    //有几个字符与输入法有关；
    const char *punc=" ～！@＃¥％……&＊（）——＋＝－｀｜、｝］｛［“‘：；？／》。《，";
    int len=strlen(punc);
    
    
    string::iterator it;
    
    
    for(int i=0; i<len; i++){
//        str.erase(0, len);
//        if( *(punc+i)== ch ){
//            return true;
//        }
        CCLog("%c+++", *(punc+i));
        
        string::iterator it;
//        for(it = m_str.begin(); it != m_str.end();){
//            if((*it >= '0' && *it <= '9') || (*it >= 'a' && *it <= 'z') || (*it >= 'A' && *it <= 'Z')){
//                CCLog("IT::%c", *it);
//                ++it;
//                
//            }else{
//                
//                if (*(punc+i)== *it){
//                    CCLog("%c===", *it);
//                    CCLog("%c+++", *(punc+i));
//                    //m_str.erase(it);
//                    return  false;
//                    
//                }else{
//                    ++it;
//                }
//            }
//        }
    }
    
    return true;
}

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
    scheduleUpdate();

    return true;
}
void RegistScene::pinyinMove() {
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
    area,
    nickname,
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

void RegistScene::checkKeyBoard() {
    bool km = CCUserDefault::sharedUserDefault()->getBoolForKey("keyboardMove");
    if(km) {

        CCUserDefault::sharedUserDefault()->setBoolForKey("keyboardMove", false);
        int kheight = CCUserDefault::sharedUserDefault()->getIntegerForKey("keyboardHeight");
        ksize.height = kheight;
        pinyinMove();
    }

}

void RegistScene::update(float diff) {
    checkKeyBoard();
}
void centerTemp(CCNode *node) {
    CCSize vs = CCDirector::sharedDirector()->getVisibleSize();
    CCSize ds = CCSizeMake(640, 960);
    float sca = std::min(vs.width/ds.width, vs.height/ds.height);
    float cx = ds.width/2;
    float cy = ds.height/2;
    float nx = vs.width/2-cx*sca;
    float ny = vs.height/2-cy*sca;

    node->setScale(sca);
    node->setPosition(ccp(nx, ny));

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
    
    
    //根Panel
    UIPanel* rootReferee = (UIPanel*)UIHelper::seekWidgetByName(addshow, "setupRefereePanel");
    
    midView = (UIPanel*)UIHelper::seekWidgetByName(addshow, "midView");
    midView->setCloseIME(true);

    //rootReferee->setEnabled(true);
    //rootReferee->setVisible(true);

    //addshow->getChildByTag(305);
    
    UIPanel* rootPhone = (UIPanel*)UIHelper::seekWidgetByName(addshow, "setupPhonePanel");
    //addshow->getChildByTag(395);
    UIPanel* rootVerify = (UIPanel*)UIHelper::seekWidgetByName(addshow, "setupVerifyPanel");//addshow->getChildByTag(374);
    //UIPanel* rootPerfect = (UIPanel*)addshow->getChildByTag(44);
    //UIPanel *rootReferee = (UIPanel*)UIHelper::seekWidgetByName(addshow, "setupRefereePanel");

    rootPerfect = (UIPanel*)UIHelper::seekWidgetByName(addshow, "setup2Panel");
    UIPanel *setBody = (UIPanel*)UIHelper::seekWidgetByName(rootPerfect, "bodyPanel");
    setBody->setCloseIME(true);

    //UIPanel* setup2 = (UIPanel*)UIHelper::seekWidgetByName(rootPerfect, "bodyPanel");

        //rootPerfect->getChildByTag(416);
    //setup2->addNode(coverView);


    UIPanel* rootConfirm = (UIPanel*)UIHelper::seekWidgetByName(addshow, "confirmPanel");//addshow->getChildByTag(466);
    rootConfirm->setEnabled(false);
    
    UIPanel *scalePanel = (UIPanel*)UIHelper::seekWidgetByName(addshow, "scalePanel");
    centerTemp(scalePanel);

    /*
    CCSize vs = CCDirector::sharedDirector()->getVisibleSize();
    float scale = std::min(vs.width/640, vs.height/960);
    rootConfirm->setScale(scale);
    */

    //可视区域
//    UIPanel* setup1 = (UIPanel*)addshow->getChildByTag(20);
    UIPanel* setup2 = (UIPanel*)UIHelper::seekWidgetByName(rootPerfect, "bodyPanel");//addshow->getChildByTag(44)->getChildByTag(416);
    UIPanel* setupReferee = (UIPanel*)UIHelper::seekWidgetByName(rootReferee, "bodyPanel");//addshow->getChildByTag(305)->getChildByTag(344);
    UIPanel* setupPhone = (UIPanel*)addshow->getChildByTag(395)->getChildByTag(398);
    UIPanel* setupVerify = (UIPanel*)addshow->getChildByTag(374)->getChildByTag(377);
    UIPanel* setupConfirm = (UIPanel*)addshow->getChildByTag(466)->getChildByTag(467);
    CCLog("init setupConfirm");

    
    //32强
    //UILabel* likeLabel = (UILabel*)addshow->getChildByTag(44)->getChildByTag(416)->getChildByTag(89)->getChildByTag(146);
    //likeLabel->addTouchEventListener(this, toucheventselector(RegistScene::worldCupStrongPress));
    
    //UIPanel* listPanel = (UIPanel*)addshow->getChildByTag(102);
    //UIListView* listView = (UIListView*)listPanel->getChildByTag(110);
    //listPanel->setEnabled(false);
    //Layout* default_item = Layout::create();
    //default_item->setTouchEnabled(true);UITextar
    //default_item->setSize(CCSizeMake(640, 30*75));
    
    //listView->setItemModel(default_item);
    
//    for(int i=0; i<30; i++){
//        UIButton* m_option = UIButton::create();
//        m_option->setTouchEnabled(true);
//        m_option->loadTextures("Scene/RegistScene/bottom_line.png", "Scene/RegistScene/bottom_line.png", "Scene/RegistScene/bottom_line.png");
//        //default_item->addChild(m_option);
//        UILabel* m_label = UILabel::create();
//        if(i % 2 == 0){
//            m_label->setText("中华人民共和国");
//        }else{
//            m_label->setText("中国");
//        }
//        
//        m_label->setFontSize(26);
//        m_label->setColor(ccc3(0,0,0));
//        m_label->setAnchorPoint(ccp(0, 0.5));
//        m_label->setPosition(ccp(-310,0));
//        m_option->addChild(m_label);
//        
//        char buffer[10];
//        sprintf(buffer, "%d", i);
//        m_option->setName(buffer);
//        
//        m_option->addTouchEventListener(this, toucheventselector(RegistScene::listviewPress));
//        
//        listView->pushBackCustomItem(m_option);
//    }
    
    /*** 按钮 ***/
    //推荐人确定按钮
    m_reffereeBtn =(UIButton*)UIHelper::seekWidgetByName(setupReferee, "finishButton");

    //setupReferee->getChildByTag(314);
    m_reffereeBtn->addTouchEventListener(this, toucheventselector(RegistScene::refereePress));
    //m_reffereeBtn->setCloseIME(true);

    //手机号码确认
    m_vphoneBtn = (UIButton*)UIHelper::seekWidgetByName(setupPhone, "finishButton");
    //setupPhone->getChildByTag(399);
    m_vphoneBtn->addTouchEventListener(this, toucheventselector(RegistScene::vphonePress));
    
    CCLog("init button");

    
    //m_vphoneBtn->addTouchEventListener(this, toucheventselector(RegistScene::cfmOkPress));
    
    
    //验证码确认
    m_verifyBtn = (UIButton*)setupVerify->getChildByTag(378);
    m_verifyBtn->addTouchEventListener(this, toucheventselector(RegistScene::verifyPress));
    //重发验证码
    UILabel* m_reSendCode = (UILabel*)setupVerify->getChildByTag(384);
    m_reSendCode->addTouchEventListener(this, toucheventselector(RegistScene::resendCodePress));
    
    CCLog("init label");

    
    //注册按钮
//    m_registBtn = (UIButton*)addshow->getChildByTag(20)->getChildByTag(14);
//    m_registBtn->addTouchEventListener(this,toucheventselector(RegistScene::registPress));
    //完成注册按钮
    UIButton* m_finishBtn = (UIButton*)UIHelper::seekWidgetByName(setup2, "finishButton");
    //setup2->getChildByTag(94);

    m_finishBtn->addTouchEventListener(this, toucheventselector(RegistScene::registPress));
    
    //返回按钮
    UIButton* m_comebackBtn = (UIButton*)addshow->getChildByTag(305)->getChildByTag(315)->getChildByTag(465);
    m_comebackBtn->addTouchEventListener(this, toucheventselector(RegistScene::comebackPress));
    
    //确认发送短信
    UIButton* m_cfmOkBtn = (UIButton*)UIHelper::seekWidgetByName(rootConfirm, "okButton"); //addshow->getChildByTag(466)->getChildByTag(467)->getChildByTag(474);
    m_cfmOkBtn->addTouchEventListener(this, toucheventselector(RegistScene::cfmOkPress));
    //取消发送短信
    UIButton* m_cfmCancelBtn = (UIButton*)UIHelper::seekWidgetByName(rootConfirm, "cancelButton"); //addshow->getChildByTag(466)->getChildByTag(467)->getChildByTag(473);
    m_cfmCancelBtn->addTouchEventListener(this, toucheventselector(RegistScene::cfmCancelPress));
    //背景点击
    //rootConfirm->addTouchEventListener(this, toucheventselector(RegistScene::cfmBgPress));
    //阅读条款按钮
    UILabel* m_provisionBtn = (UILabel*)setupPhone->getChildByTag(405)->getChildByTag(409);
    m_provisionBtn->setTouchEnabled(true);
    m_provisionBtn->addTouchEventListener(this, toucheventselector(RegistScene::provisionPress));

    //性别复选框
    UICheckBox* manCheckBox = (UICheckBox*)UIHelper::seekWidgetByName(setup2, "sexCheckBox1");//addshow->getChildByTag(44)->getChildByTag(416)->getChildByTag(224);
    UICheckBox* WomCheckBox = (UICheckBox*)UIHelper::seekWidgetByName(setup2, "sexCheckBox2");//addshow->getChildByTag(44)->getChildByTag(416)->getChildByTag(227);
    
    manCheckBox->addEventListenerCheckBox(this, checkboxselectedeventselector(RegistScene::sexCheckBoxPress));
    WomCheckBox->addEventListenerCheckBox(this, checkboxselectedeventselector(RegistScene::sexCheckBoxPress));
    manCheckBox->setSelectedState(true);
    
    CCLog("init checkBox");
    

    //推荐人手机号
    
    /*
    m_itrsPhoneNum = CCEditBox::create(CCSize(500,45),CCScale9Sprite::createWithSpriteFrameName("button.png"));
    m_itrsPhoneNum->setMaxLength(11);
	m_itrsPhoneNum->setFontSize(23);
	//m_itrsPhoneNum->setAnchorPoint(CCPointZero);
	m_itrsPhoneNum->setPlaceHolder("请输入推荐人手机号");
	m_itrsPhoneNum->setInputMode(kEditBoxInputModePhoneNumber);
	m_itrsPhoneNum->setReturnType(kKeyboardReturnTypeDone);

    m_itrsPhoneNum->setFontColor(ccc3(80,80,80));
	m_itrsPhoneNum->setDelegate(this);
	m_itrsPhoneNum->setTouchPriority(0);
    m_itrsPhoneNum->setAnchorPoint(ccp(0.5, 0));
    */



    //
    //UITextField* 
    refPhoneNum = (UITextField*)UIHelper::seekWidgetByName(addshow, "refPhoneNum");
    refPhoneNum->setInputType(1);
    refPhoneNum->setTextHorizontalAlignment(kCCTextAlignmentCenter);

    UIPanel* setupRefereePanel = (UIPanel*)UIHelper::seekWidgetByName(addshow, "setupRefereePanel");
    //addshow->getChildByTag(305)
    UILabel *bodyPanel = (UILabel*)UIHelper::seekWidgetByName(setupRefereePanel, "bodyPanel");
    bodyPanel->setCloseIME(true);

    //refPhoneNum->setEnabled(false);
    //refPhoneNum->setVisible(false);

    CCLog("init refPhoneNum");

    //


    //验证码
    /*
    m_verityNum = CCEditBox::create(CCSize(500,45),CCScale9Sprite::createWithSpriteFrameName("button.png"));
    m_verityNum->setMaxLength(11);
	m_verityNum->setFontSize(23);
	//m_verityNum->setAnchorPoint(CCPointZero);
	m_verityNum->setPlaceHolder("请输入验证码");
	m_verityNum->setInputMode(kEditBoxInputModePhoneNumber);
	m_verityNum->setReturnType(kKeyboardReturnTypeDone);
    m_verityNum->setFontColor(ccc3(80,80,80));
	m_verityNum->setDelegate(this);
	m_verityNum->setTouchPriority(0);
    m_verityNum->setAnchorPoint(ccp(0.5, 0));
    */

    verifyCode = (UITextField*)UIHelper::seekWidgetByName(addshow, "verifyCode");
    verifyCode->setInputType(1);
    verifyCode->setTextHorizontalAlignment(kCCTextAlignmentCenter);

    //verifyCode->setInputType(1);


    //手机号码
    /*
    m_phoneNum = CCEditBox::create(CCSize(500,45), CCScale9Sprite::createWithSpriteFrameName("button.png"));
	m_phoneNum->setFontSize(23);
    m_phoneNum->setMaxLength(11);
	//m_phoneNum->setAnchorPoint(CCPointZero);
	m_phoneNum->setPlaceHolder("请输入手机号");
	m_phoneNum->setInputMode(kEditBoxInputModePhoneNumber);
	m_phoneNum->setReturnType(kKeyboardReturnTypeDone);
    m_phoneNum->setFontColor(ccc3(80,80,80));
	m_phoneNum->setPosition(ccp(180,767));
	m_phoneNum->setDelegate(this);
	m_phoneNum->setTouchPriority(0);
    m_phoneNum->setAnchorPoint(ccp(0.5, 0));
    */

    myPhoneNum = (UITextField*)UIHelper::seekWidgetByName(addshow, "myPhoneNum");
    myPhoneNum->setInputType(1);
    myPhoneNum->setTextHorizontalAlignment(kCCTextAlignmentCenter);
    


    //密码
    /*
    m_password = CCEditBox::create(CCSize(500,45),CCScale9Sprite::createWithSpriteFrameName("button.png"));
	m_password->setFontSize(23);
    m_password->setMaxLength(16);
	m_password->setAnchorPoint(CCPointZero);
	m_password->setPlaceHolder("请输入密码");
	m_password->setInputMode(kEditBoxInputModeAny);
	m_password->setInputFlag(kEditBoxInputFlagPassword);
	m_password->setReturnType(kKeyboardReturnTypeDone);
    m_password->setFontColor(ccc3(80,80,80));
	m_password->setPosition(ccp(180,663));
	m_password->setDelegate(this);
	m_password->setTouchPriority(0);
    */

    password = (UITextField*)UIHelper::seekWidgetByName(rootPerfect, "password");


    //确认密码
//    m_cfmPassWord = CCEditBox::create(CCSize(300,50),CCScale9Sprite::createWithSpriteFrameName("button.png"));
//	m_cfmPassWord->setFontSize(23);
//	m_cfmPassWord->setAnchorPoint(CCPointZero);
//	m_cfmPassWord->setPlaceHolder("请确认密码");
//	m_cfmPassWord->setInputMode(kEditBoxInputModeAny);
//	m_cfmPassWord->setInputFlag(kEditBoxInputFlagPassword);
//	m_cfmPassWord->setReturnType(kKeyboardReturnTypeDone);
//    m_cfmPassWord->setFontColor(ccc3(80,80,80));
//	m_cfmPassWord->setPosition(ccp(180,561));
//	m_cfmPassWord->setDelegate(this);
//	m_cfmPassWord->setTouchPriority(0);

     //真实姓名
    /*
     m_realName = CCEditBox::create(CCSize(500,45),CCScale9Sprite::createWithSpriteFrameName("button.png"));
     m_realName->setFontSize(23);
     m_realName->setAnchorPoint(CCPointZero);
     m_realName->setPlaceHolder("请输入姓名");
     m_realName->setMaxLength(6);
     m_realName->setInputMode(kEditBoxInputModeAny);
     m_realName->setReturnType(kKeyboardReturnTypeDone);
     m_realName->setFontColor(ccc3(80,80,80));
     m_realName->setPosition(ccp(180,522));
     m_realName->setDelegate(this);
     m_realName->setTouchPriority(0);
    */

    nickname = (UITextField*)UIHelper::seekWidgetByName(rootPerfect, "nickName");

    
    //地区
    /*
    m_areaBox = CCEditBox::create(CCSize(500,45),CCScale9Sprite::createWithSpriteFrameName("button.png"));
    m_areaBox->setFontSize(23);
    m_areaBox->setAnchorPoint(CCPointZero);
    m_areaBox->setPlaceHolder("请输入地区");
    m_areaBox->setMaxLength(12);
    m_areaBox->setInputMode(kEditBoxInputModeAny);
    m_areaBox->setReturnType(kKeyboardReturnTypeDone);
    m_areaBox->setFontColor(ccc3(80,80,80));
    m_areaBox->setPosition(ccp(180,366));
    m_areaBox->setDelegate(this);
    m_areaBox->setTouchPriority(0);
    */

    area = (UITextField*)UIHelper::seekWidgetByName(rootPerfect, "area");
    
    
    /*** 设置输入框位置 ***/
    //推荐人
    UIImageView* refereeLiimage = (UIImageView*)setupReferee->getChildByTag(335);
    //setupReferee->addNode(m_itrsPhoneNum);
    //refereeLiimage->getParent()->addChild(m_itrsPhoneNum);
    //UILabel *bodyPanel = (UILabel*)UIHelper::seekWidgetByName(addshow, "bodyPanel");
    //bodyPanel->addChild(m_itrsPhoneNum);
    //bodyPanel->addNode(m_itrsPhoneNum);
    
    //refereeLiimage->addNode(m_itrsPhoneNum);
    
    
    


    CCPoint refereePoint = refereeLiimage->getPosition();
    
    CCLog("referre image pos %f %f", refereePoint.x, refereePoint.y);

    //setupReferee->addNode(m_itrsPhoneNum);
    //m_itrsPhoneNum->setPosition(ccp(refereePoint.x, refereePoint.y - 15));
    
    CCSize sr = setupReferee->getSize();
    CCLog("back size %f %f", sr.width, sr.height);


    
    //手机号码
    UIImageView* phoneLiimage = (UIImageView*)setupPhone->getChildByTag(402);
    //setupPhone->addNode(m_phoneNum);
    CCPoint phonePoint = phoneLiimage->getPosition();
    //m_phoneNum->setPosition(ccp(phonePoint.x, phonePoint.y - 15));

    
    //验证码
    UIImageView* verityLiimage = (UIImageView*)setupVerify->getChildByTag(381);
    //setupVerify->addNode(m_verityNum);
    CCPoint verifyPoint = verityLiimage->getPosition();
    //m_verityNum->setPosition(ccp(verifyPoint.x, verifyPoint.y - 15));
    

    //密码
    UIImageView* passwordLiImage = (UIImageView*)UIHelper::seekWidgetByName(setup2, "passwordLiImage");//setup2->getChildByTag(172);
    //setup2->addNode(m_password);
    CCPoint passwordPoint = passwordLiImage->getPosition();
    //m_password->setPosition(ccp(passwordPoint.x -190, passwordPoint.y -15));
    
    //昵称
    ImageView* usernameLiImage = (ImageView*)UIHelper::seekWidgetByName(setup2, "usernameLiImage");//setup2->getChildByTag(191);
    //setup2->addNode(m_realName);
    CCPoint usernamePoint = usernameLiImage->getPosition();
    //m_realName->setPosition(ccp(usernamePoint.x-190, usernamePoint.y-15));
    
    //区域
    ImageView* areaLiImage = (ImageView*)UIHelper::seekWidgetByName(setup2, "arealLiImage");//setup2->getChildByTag(193);
    //setup2->addNode(m_areaBox);
    CCPoint areaPoint = areaLiImage->getPosition();
    //m_areaBox->setPosition(ccp(areaPoint.x-190, areaPoint.y-15));

    
    showPanel(1);

    //showPanel(4);

    
    //如果只是资料未填写跳到完善资料
    int setPerfectFormId = CCUserDefault::sharedUserDefault()->getIntegerForKey("setPerfectFormId");
    CCLog("setPerfectFormId::%d",setPerfectFormId);
    if( setPerfectFormId > 0){
        CCUserDefault::sharedUserDefault()->setIntegerForKey("setPerfectFormId", 0);
        CCUserDefault::sharedUserDefault()->flush();
        
        v_userid = setPerfectFormId;
        string tmp_loginName = CCUserDefault::sharedUserDefault()->getStringForKey("loginName");
        //账号label
        v_phoneNum = tmp_loginName;
        showPanel(4);
    }


    //showPanel(4);
}

/*** 按钮事件回调 ***/

//推荐人
void RegistScene::refereePress( CCObject *pSender,TouchEventType type )
{
    
	if (type == TOUCH_EVENT_ENDED)
	{
        //v_itrsPhoneNum = m_itrsPhoneNum->getText();
        
        v_itrsPhoneNum = refPhoneNum->getStringValue();

        
        if(v_itrsPhoneNum == ""){
            CCMessageBox("所有选项不能为空!", "提示");
            return;
        }
        for (int i = 0; i < v_itrsPhoneNum.size(); ++i)
        {
            if (v_itrsPhoneNum[i] < '0' or v_itrsPhoneNum[i] > '9')
            {
                CCMessageBox("手机号码只可以出现数字", "提示");
                return;
            }
        }
        UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
        pUserService->refereeNum(v_itrsPhoneNum, this, MyHttpResp(&RegistScene::refereeEnd));
        //showPanel(2);
        CCLog("refereePress btn click!");
	}
}

//确认手机
void RegistScene::vphonePress( CCObject *pSender,TouchEventType type )
{
    
	if (type == TOUCH_EVENT_ENDED)
	{
//        v_phoneNum = m_phoneNum->getText();
//
//        if(v_phoneNum == ""){
//            CCMessageBox("所有选项不能为空!", "提示");
//            return;
//        }
//        
//        //showPanel(3);
//        char refereeIdItem[50];
//        sprintf(refereeIdItem, "%d", v_refereeId);
//        UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
//        pUserService->phoneNum(v_phoneNum, refereeIdItem, this, MyHttpResp(&RegistScene::vphoneEnd));
        
        //v_phoneNum = m_phoneNum->getText();
        v_phoneNum = myPhoneNum->getStringValue();

        if(v_phoneNum == ""){
            CCMessageBox("所有选项不能为空!", "提示");
            return;
        }
        
        UIPanel* rootConfirm = (UIPanel*)addshow->getChildByTag(466);
        rootConfirm->setEnabled(true);
        rootConfirm->setVisible(true);

        
        UILabel* phoneLable = (UILabel*)UIHelper::seekWidgetByName(rootConfirm, "phoneNumLabel");//rootConfirm->getChildByTag(467)->getChildByTag(505);
        phoneLable->setText(v_phoneNum);
        
        CCLog("vphonePress btn click!");
	}
}

//确认验证码
void RegistScene::verifyPress( CCObject *pSender,TouchEventType type )
{
    
	if (type == TOUCH_EVENT_ENDED)
	{
        //showPanel(4);
        //v_verityNum = m_verityNum->getText();

        v_verityNum = verifyCode->getStringValue();
        
        if(v_verityNum == ""){
            CCMessageBox("所有选项不能为空!", "提示");
            return;
        }
        
        //just For Test
        //showPanel(4);

        
        
        char refereeIdItem[50];
        sprintf(refereeIdItem, "%d", v_refereeId);
        UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
        pUserService->verify(v_phoneNum, refereeIdItem, v_verityNum, this, MyHttpResp(&RegistScene::verifyEnd));
        

        CCLog("verifyPress btn click!");
	   
    }
}

//重发验证码
void RegistScene::resendCodePress( CCObject *pSender,TouchEventType type )
{
    
	if (type == TOUCH_EVENT_ENDED)
	{
        if(v_phoneNum == ""){
            CCMessageBox("所有选项不能为空!", "提示");
            return;
        }
        
        //showPanel(3);
        char refereeIdItem[50];
        sprintf(refereeIdItem, "%d", v_refereeId);
        UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
        pUserService->phoneNum(v_phoneNum, refereeIdItem, this, MyHttpResp(&RegistScene::vResendPhoneEnd));
        
        CCLog("resendCodePress btn click!");
	}
}

//注册
void RegistScene::registPress( CCObject *pSender,TouchEventType type )
{
    
	if (type == TOUCH_EVENT_ENDED)
	{
        CCLog("register Press getValue");
        //v_password = m_password->getText();
        CCLog("password, nickname, ares %x %x %x", password, nickname, area);

        v_password = password->getStringValue();

        //string v_nick = m_realName->getText();
        string v_nick = nickname->getStringValue();
        //string v_area = m_areaBox->getText();

        string v_area = area->getStringValue();


        
        //选项不为空
        if(v_password=="" || trim(v_nick)=="" || trim(v_area) == ""){
            CCMessageBox("所有选项不能为空！", "提示");
            return;
        }
        
        if(!checkTextString(v_nick)){
            CCMessageBox("昵称错误（只能包含数字、字母、中文）", "提示");
            return;
        }
        
        if(v_password.size() < 4){
            CCMessageBox("密码长度请大于四个字符！", "安全提示");
            return;
        }
        
        //喜欢的36强
        int likeNum = coverView->getCurCardIndex();
        char likeItem[50];
        sprintf(likeItem, "%d", likeNum + 1);
        
        //性别
        CCLog("get gender ");

        string m_gender;
        UICheckBox* manCheckBox = (UICheckBox*)UIHelper::seekWidgetByName(rootPerfect, "sexCheckBox1");//addshow->getChildByTag(44)->getChildByTag(416)->getChildByTag(224);
        if(manCheckBox->getSelectedState()){
            m_gender = "1";
        }else{
            m_gender = "0";
        }
        
		UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
		pUserService->registe(v_phoneNum, md5(v_password), v_nick, m_gender, v_area, likeItem, this,MyHttpResp(&RegistScene::registEnd));
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
void RegistScene::cfmOkPress(CCObject* pSender, TouchEventType type)
{
    
	if (type == TOUCH_EVENT_ENDED)
	{
        //v_phoneNum = m_phoneNum->getText();
        
        if(v_phoneNum == ""){
            CCMessageBox("所有选项不能为空!", "提示");
            return;
        }
        
        //showPanel(3);
        char refereeIdItem[50];
        sprintf(refereeIdItem, "%d", v_refereeId);
        UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
        pUserService->phoneNum(v_phoneNum, refereeIdItem, this, MyHttpResp(&RegistScene::vphoneEnd));
        CCLog("cfmOkPress btn click!");
	}
}
void RegistScene::cfmCancelPress(CCObject* pSender, TouchEventType type)
{
    
	if (type == TOUCH_EVENT_ENDED)
	{
        UIPanel* rootConfirm = (UIPanel*)addshow->getChildByTag(466);
        rootConfirm->setEnabled(false);
        CCLog("cfmCancelPress btn click!");
	}
}
void RegistScene::provisionPress(CCObject* pSender, TouchEventType type)
{
    
	if (type == TOUCH_EVENT_ENDED)
	{
        //CCApplication::sharedApplication()->openURL("http://113.108.228.190:91/support/copyright");
        CCLog("provisionPress btn click!");
	}
}

void RegistScene::cfmBgPress(CCObject* pSender, TouchEventType type)
{
    
	if (type == TOUCH_EVENT_ENDED)
	{
        
        UIPanel* rootConfirm = (UIPanel*)addshow->getChildByTag(466);
        rootConfirm->setEnabled(false);
	}
}

//checkbox event
void RegistScene::sexCheckBoxPress(CCObject* pSender, CheckBoxEventType type){
    //性别复选框
    UICheckBox* manCheckBox = (UICheckBox*)UIHelper::seekWidgetByName(rootPerfect, "sexCheckBox1");//addshow->getChildByTag(44)->getChildByTag(416)->getChildByTag(224);
    UICheckBox* WomCheckBox = (UICheckBox*)UIHelper::seekWidgetByName(rootPerfect, "sexCheckBox2");;//addshow->getChildByTag(44)->getChildByTag(416)->getChildByTag(227);
    
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
        //UserService::shareUserService()->registe(v_phoneNum, md5(v_password), md5(v_cfmPassword), m_itrsPhoneNum->getText(),this,MyHttpResp(&RegistScene::registEnd));
        //CCLOG("finish");
        
        int likeNum = coverView->getCurCardIndex();
        char likeItem[50];
        sprintf(likeItem, "%d", likeNum + 1);
        char useridItem[50];
        sprintf(useridItem, "%d", v_userid);
        
        //string  realName = m_realName->getText();
        string realName = nickname->getStringValue();

        //string  areaBox = m_areaBox->getText();
        string  areaBox = area->getStringValue();

        if(realName == "" || areaBox == ""){
            CCMessageBox("所有选项不能为空！", "提示");
            return;
        }
        
        string m_gender;
        UICheckBox* manCheckBox = (UICheckBox*)UIHelper::seekWidgetByName(rootPerfect, "sexCheckBox1");//addshow->getChildByTag(44)->getChildByTag(416)->getChildByTag(224);
        if(manCheckBox->getSelectedState()){
            m_gender = "1";
        }else{
            m_gender = "0";
        }
		UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
		pUserService->setPerfect(useridItem, realName, m_gender, areaBox, likeItem, this, MyHttpResp(&RegistScene::setPerfectEnd));
        pUserService->getUser()->uid = v_userid;
        CCLog("set perfect");
        

	}
}

//修改球队头像
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

        
        //UILabel* likeLabel = (UILabel*)addshow->getChildByTag(44)->getChildByTag(416)->getChildByTag(89)->getChildByTag(146);
        //likeLabel->setText(m_btn->getName());
	}
}

void RegistScene::setPerfectEnd(bool suc, std::string s, void*param){
    if(suc == false){
        return;
    }
    UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
    bool isPerfected = pUserService->analyzePerfectRect(s);
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
    CCScene* pScene = WorldCup::scene();
    pDirector->replaceScene(pScene);


    CCIMEDispatcher::sharedDispatcher()->closeIME();
    
    //Logic::getInstance()->setUID();
    CCLog("setPerfectEnd!");
}
//推荐人
void RegistScene::refereeEnd(bool suc, std::string s, void*param){
    
    CCLog("refereeEnd");
    if(suc == false){
        return;
    }
    UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
    NetReferrer mData = pUserService->analyzeRefereeRect(s);
    if(mData.status == 1){
        v_refereeId = mData.referrerId;
        showPanel(2);
    }else{
        CCMessageBox(mData.msg.c_str(), "提示");
    }
}
//确认手机
void RegistScene::vphoneEnd(bool suc, std::string s, void*param){
    if(suc == false){
        return;
    }
    CCLog("vphoneEnd");
    
    UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
    NetBase mData = pUserService->analyzeAuthCodeRect(s);
    if(mData.status == 1){
//        string msg = "我们将发送验证码短信到这个号码:\n" + v_phoneNum;
//        CCMessageBox(msg.c_str(), "手机号码确认");
        
        UIPanel* rootConfirm = (UIPanel*)addshow->getChildByTag(466);
        rootConfirm->setEnabled(false);
        
        showPanel(3);
    }else{
        CCMessageBox(mData.msg.c_str(), "提示");
    }
}

//重发验证码
void RegistScene::vResendPhoneEnd(bool suc, std::string s, void*param){
    
    CCLog("vResendPhoneEnd");
    if(suc == false){
        return;
    }
    UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
    NetBase mData = pUserService->analyzeAuthCodeRect(s);
    if(mData.status == 1){
        CCMessageBox("重发验证码成功！", "提示");
    }else{
        CCMessageBox(mData.msg.c_str(), "提示");
    }
}

//确认验证码
void RegistScene::verifyEnd(bool suc, std::string s, void*param){
    
    CCLog("vphoneEnd");
    if(suc == false){
        return;
    }
    UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
    NetBase mData = pUserService->analyzeAuthCodeRect(s);
    if(mData.status == 1){
        showPanel(4);
    }else{
        CCMessageBox(mData.msg.c_str(), "提示");
    }
}


void RegistScene::registEnd(bool suc, std::string s, void*param)
{
    if(suc == false){
        return;
    }
    
    UserService* pUserService = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
    NetRegist regData = pUserService->analyzeRegistRect(s);
    CCLog("net regist %d", regData.status);
    if(regData.status == 1){
        //CCDirector* pDirector = CCDirector::sharedDirector();
        
        //存储数据到Logic里面即可
        /*
        loginName=123456789&password=c4ca4238a0b923820dcc509a6f75849b&passwordconfirm=c4ca4238a0b923820dcc509a6f75849b&referrer=13128513872
         Cocos2d: finish
         Cocos2d: test request finish
        {"state":1,"data":{"id":30}}
        */
        //返回的数据
        
        /*
        rapidjson::Document d;
        d.Parse<0>(s.c_str());
        const rapidjson::Value &rdata = d["data"];
        Logic *lg = Logic::getInstance();
        //注册的时候 提交的 用户信息 喜欢的球队信息
        
        lg->setUID(rdata["id"].GetInt());
        lg->setLoginName(v_phoneNum);
        
        lg->setBio(m_itrsPhoneNum->getText());
        
        CCScene* pScene = WorldCup::scene();
        pDirector->replaceScene(pScene);
        */
        
        
        //v_userid = regData.userid;
        //Logic *lg = Logic::getInstance();
        //注册的时候 提交的 用户信息 喜欢的球队信息
        
        //lg->setUID(regData.userid);
        pUserService->getUser()->uid = regData.userid;
        //注册成功进入到完善资料
        //showPanel(2);
        CCMessageBox("恭喜您注册成功！", "提示");
        
        //完成暂时跳转到登陆界面
        //保存本地用户名
        CCUserDefault::sharedUserDefault()->setStringForKey("loginName", v_phoneNum);
        CCUserDefault::sharedUserDefault()->setIntegerForKey("avatar", coverView->getCurCardIndex() + 1);
        CCUserDefault::sharedUserDefault()->flush();
        CCDirector* pDirector = CCDirector::sharedDirector();
        CCScene* pScene = WorldCup::scene();
        pDirector->replaceScene(pScene);
        
    }else{
        CCMessageBox(regData.msg.c_str(), "提示");
    }
    //缓存到内存中
    //GloabData::shareGloabData()->self_User = user;
    
}

//32强
void RegistScene::worldCupStrongPress( CCObject *pSender,TouchEventType type )
{
    
	if (type == TOUCH_EVENT_ENDED)
	{
        UIPanel* listPanel = (UIPanel*)UIHelper::seekWidgetByName(addshow, "listPanel");
        //addshow->getChildByTag(102);

        listPanel->setEnabled(true);
        listPanel->setVisible(true);
	}
}


void RegistScene::showPanel(int setup){

    //disable时候关闭输入框
    //切换页面 关闭输入框
    CCIMEDispatcher::sharedDispatcher()->closeIME();   
    //根Panel
    UIPanel* rootReferee = (UIPanel*)UIHelper::seekWidgetByName(addshow, "setupRefereePanel");

    //addshow->getChildByTag(305);

    UIPanel* rootPhone = (UIPanel*)UIHelper::seekWidgetByName(addshow, "setupPhonePanel");
    UIPanel *bphone = (UIPanel*)UIHelper::seekWidgetByName(rootPhone, "bodyPanel");
    bphone->setCloseIME(true);

    //addshow->getChildByTag(395);
    UIPanel* rootVerify = (UIPanel*)UIHelper::seekWidgetByName(addshow, "setupVerifyPanel");
    UIPanel *bphone2 = (UIPanel*)UIHelper::seekWidgetByName(rootVerify, "bodyPanel");
    bphone2->setCloseIME(true);

    //addshow->getChildByTag(374);
    UIPanel* rootPerfect = (UIPanel*)UIHelper::seekWidgetByName(addshow, "setup2Panel");

    
    
    //addshow->getChildByTag(44);
    UIPanel *listPanel = (UIPanel*)UIHelper::seekWidgetByName(addshow, "listPanel");
    //隐藏面板
    rootReferee->setEnabled(false);
    rootPhone->setEnabled(false);
    rootVerify->setEnabled(false);
    rootPerfect->setEnabled(false);
    
    //listPanel->setVisible(false);



    //m_phoneNum->setEnabled(false);
    
    //m_password->setEnabled(false);
    
    //m_cfmPassWord->setEnabled(false);
    

    //m_itrsPhoneNum->setEnabled(false);


    //m_realName->setEnabled(false);
    //m_areaBox->setEnabled(false);
    
    //m_verityNum->setEnabled(false);
    
    if(setup == 1){
        //m_itrsPhoneNum->setEnabled(true);
        
        rootReferee->setEnabled(true);
        rootReferee->setVisible(true);
    }else if(setup == 2){
        //m_phoneNum->setEnabled(true);
        
        rootPhone->setEnabled(true);
        rootPhone->setVisible(true);

    }else if(setup == 3){
        
        UILabel* phoneLable = (UILabel*)addshow->getChildByTag(374)->getChildByTag(377)->getChildByTag(380);
        phoneLable->setText(v_phoneNum);
        
        //m_verityNum->setEnabled(true);
        
        rootVerify->setEnabled(true);
        rootVerify->setVisible(true);

    }else if(setup == 4){
        
        //coverView
        float v_height = CCDirector::sharedDirector()->getVisibleSize().height;
        float v_width = CCDirector::sharedDirector()->getVisibleSize().width;
        //缩放比例
        float scale = std::min(v_width/640, v_height/960);
        /*
        if (scale >= 1)
        {
            scale = 1;
        }
        */

        coverView = CoverView::create(CCRect((v_width-500)/2,v_height - 960 + 690,500, 400), CCSize(127, 127), 180, 0.25f);
        
        coverView->setPosition((v_width-500*scale)/2, 690*scale);
        coverView->setScale(scale);

        

        for(int i=1; i<=32;i++){
            char iconPath[100];
            sprintf(iconPath, "flags/%d.png", i);
            CCSprite* player = CCSprite::create(iconPath);
            CCSize m_size = player->getContentSize();
            float s_x = 110 / m_size.width;
            float s_y = 110 / m_size.height;
            player->setScaleX(s_x);
            player->setScaleY(s_y);
            player->setAnchorPoint(ccp(0, 0));
            //player->setTextureRect(CCRect(0, 0, 127, 127));
//            player->setContentSize(CCSize(127,127));
            CCSprite* playerBg = CCSprite::create("img_chosenLI_bg.png");
            
            UILabel* teamName = UILabel::create();
            teamName->setText(getTeamName(i));
            teamName->setFontSize(26);
            teamName->setColor(ccc3(0,0,0));
            teamName->setPosition(ccp(50, -28));
            teamName->setZOrder(100);
            playerBg->addChild(teamName);
            
            playerBg->addChild(player);

            //根据屏幕分辨率调整比例 增加小屏幕进行测试  800*480
            playerBg->setScale(scale);


            coverView->addCard(playerBg);
        }
        coverView->setOffsetPosition(ccp(50, 50));
        
        coverView->adjusetEndScrollView();

        UIPanel* setup2 = (UIPanel*)UIHelper::seekWidgetByName(rootPerfect, "bodyPanel");

        //rootPerfect->getChildByTag(416);
        setup2->addNode(coverView);
        
        //转到setup2
        //setup1->setPosition(ccp(0, 9999));

        //setup1->setEnabled(false);
        rootPerfect->setEnabled(true);
        rootPerfect->setVisible(true);


        //m_realName->setEnabled(true);
        //m_itrsPhoneNum->setEnabled(true);
        //m_password->setEnabled(true);
        
        //m_areaBox->setEnabled(true);
    
        //账号label
        UILabel* usernameLabel = (UILabel*)UIHelper::seekWidgetByName(rootPerfect, "usernameLabel");//addshow->getChildByTag(44)->getChildByTag(416)->getChildByTag(179)->getChildByTag(187);
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
    CCLog(text.c_str());
//    editBox->setText("•");
    //editBox->setText(delSpecialChar(text).c_str());
    
}

void RegistScene::editBoxReturn( cocos2d::extension::CCEditBox* editBox )
{
    
}
void RegistScene::onExit(){
    CCLayer::onExit();
    
    
}