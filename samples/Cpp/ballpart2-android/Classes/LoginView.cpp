//
//  LoginView.cpp
//  TestCpp
//
//  Created by dozengame on 14-5-16.
//
//

#include "LoginView.h"
#include "HttpModel.h"
#include "RegisterView.h"
#include "Logic.h"
#include "WorldCup.h"
#include "Md5.h"
#include "User.h"


CCScene *LoginView::scene(){
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    LoginView *layer = LoginView::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

bool LoginView::init(){
    if ( !CCLayer::init() )
    {
        return false;
    }
    setSizeYet = false;
    inLogin = false;
    checkYet = false;
    
    CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
    CCSize size = CCDirector::sharedDirector()->getVisibleSize();
    UILayer *lay = UILayer::create();
    addChild(lay);
    
    w = GUIReader::shareReader()->widgetFromJsonFile("gui/ballUI_4.json");
    lay->addWidget(w);
    w->setSize(size);
    
    password = static_cast<TextField*>(UIHelper::seekWidgetByName(w, "password"));
    password->addEventListenerTextField(this, textfieldeventselector(LoginView::onPassword));
    
    login = static_cast<Button*>(UIHelper::seekWidgetByName(w, "login"));
    login->addTouchEventListener(this, toucheventselector(LoginView::onLogin));
    //不输入不允许登录的
    //login->setTouchEnabled(false);
    
    Layout *backp = static_cast<Layout*>(UIHelper::seekWidgetByName(w, "Panel_3"));
    backp->addTouchEventListener(this, toucheventselector(LoginView::onBackground));
    
    
    reg = static_cast<Button*>(UIHelper::seekWidgetByName(w, "register"));
    reg->addTouchEventListener(this, toucheventselector(LoginView::onReg));
    
    
    phoneNum = static_cast<TextField*>(UIHelper::seekWidgetByName(w, "TextField_9"));
    phoneNum->addEventListenerTextField(this, textfieldeventselector(LoginView::onPhonenum));
    
    error = static_cast<Label*>(UIHelper::seekWidgetByName(w, "error"));
    error->setEnabled(false);
    
    
    scheduleUpdate();
    
    CCUserDefault *u = CCUserDefault::sharedUserDefault();
    string p = u->getXMLFilePath();
    CCLog("uf path %s", p.c_str());
    
    
    
    return true;
    
}
void LoginView::onBackground(cocos2d::CCObject *obj, TouchEventType tt){
    switch (tt) {
        case cocos2d::ui::TOUCH_EVENT_BEGAN:
        {
            phoneNum->closeIME();
            password->closeIME();
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_MOVED:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_ENDED:
        {
            
        }
            
            break;
        case cocos2d::ui::TOUCH_EVENT_CANCELED:
        {
            
        }
            break;
        default:
            break;
    }
}
void LoginView::onPassword(CCObject *obj, TextFiledEventType tt) {
    switch (tt) {
        case cocos2d::ui::TEXTFIELD_EVENT_ATTACH_WITH_IME:
        {
            //showKeyboard = true;
            //CCLog("setSizeYet %d",setSizeYet);
            //CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
            //bottom->runAction(CCMoveTo::create(0.225, ccp(0, screenSize.height/2)));
            /*
            if (setSizeYet) {
                CCPoint wp = phoneNum->convertToWorldSpace(ccp(0, 0));
                CCSize ws = phoneNum->getSize();
                float offY = ksize.height+ws.height/2-wp.y;
                
                if (offY  > 0) {
                    w->runAction(CCMoveTo::create(0.225, ccp(0, offY)));
                }
                //bottom->runAction(CCMoveTo::create(0.225, ccp(0, ksize.height)));
                
            }
             */
            //CCEGLView *pegl = CCDirector::sharedDirector()->getOpenGLView();
            //pegl->setIMEKeyboardState(true);
            
        }
            break;
        case cocos2d::ui::TEXTFIELD_EVENT_DETACH_WITH_IME:
        {
            //关闭键盘
            //CCLog("event detach with ime");
            //bottom->runAction(CCMoveTo::create(0.225, ccp(0, 0)));
            //CCEGLView *pegl = CCDirector::sharedDirector()->getOpenGLView();
            //pegl->setIMEKeyboardState(false);
            //w->runAction(CCMoveTo::create(0.225, ccp(0, 0)));
        }
            break;
        case cocos2d::ui::TEXTFIELD_EVENT_INSERT_TEXT:
            break;
        default:
            break;
    }
}
void LoginView::onPhonenum(CCObject *obj, TextFiledEventType tt){
    switch (tt) {
        case cocos2d::ui::TEXTFIELD_EVENT_ATTACH_WITH_IME:
        {
            //showKeyboard = true;
            //CCLog("setSizeYet %d",setSizeYet);
            //CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
            //bottom->runAction(CCMoveTo::create(0.225, ccp(0, screenSize.height/2)));
            if (setSizeYet) {
                CCPoint wp = phoneNum->convertToWorldSpace(ccp(0, 0));
                CCSize ws = phoneNum->getSize();
                float offY = ksize.height+ws.height/2-wp.y;
                if (offY  > 0) {
                //bottom->runAction(CCMoveTo::create(0.225, ccp(0, ksize.height)));
                    w->runAction(CCMoveTo::create(0.225, ccp(0, offY)));
                }
            }
            //CCEGLView *pegl = CCDirector::sharedDirector()->getOpenGLView();
            //pegl->setIMEKeyboardState(true);
            
        }
            break;
        case cocos2d::ui::TEXTFIELD_EVENT_DETACH_WITH_IME:
        {
            //关闭键盘
            //CCLog("event detach with ime");
            //bottom->runAction(CCMoveTo::create(0.225, ccp(0, 0)));
            //CCEGLView *pegl = CCDirector::sharedDirector()->getOpenGLView();
            //pegl->setIMEKeyboardState(false);
            w->runAction(CCMoveTo::create(0.225, ccp(0, 0)));
        }
            break;
        case cocos2d::ui::TEXTFIELD_EVENT_INSERT_TEXT:
        {
            
        }
            break;
        case cocos2d::ui::TEXTFIELD_EVENT_DELETE_BACKWARD:
        {
            
        }
            break;
        default:
            break;
    }
}

void LoginView::onLogin(CCObject *obj, TouchEventType tt){
    switch (tt) {
        case cocos2d::ui::TOUCH_EVENT_BEGAN:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_MOVED:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_ENDED:
        {
            if (inLogin) {
                return;
            }
            string ln = phoneNum->getStringValue();
            string pw = password->getStringValue();
            if (ln.length() == 0) {
                error->setEnabled(true);
                error->runAction(CCSequence::create(CCScaleTo::create(0.1, 1.2, 1.2), CCScaleTo::create(0.1, 1, 1), NULL));
                error->setText("登录名不能为空");
                return;
            }
            if (pw.length() == 0) {
                error->setEnabled(true);
                error->runAction(CCSequence::create(CCScaleTo::create(0.1, 1.2, 1.2), CCScaleTo::create(0.1, 1, 1), NULL));
                error->setText("密码不能为空");
                return;
            }
            
            
            inLogin = true;
            
            //UserService::getInstance()->login(ln, pw, this, MYHTTP_SEL(LoginView::loginOver));
            
            /*
            HttpModel *hm = HttpModel::getInstance();
            std::map<string, string>pd;
            pd["loginName"] = ln;
            pd["password"] = md5(pw);
            hm->addRequest("user/login", "POST", pd, this, MYHTTP_SEL(LoginView::loginOver), NULL);
             */
        }
            
            break;
        case cocos2d::ui::TOUCH_EVENT_CANCELED:
        {
            
        }
            break;
        default:
            break;
    }
}

void LoginView::loginOver(bool suc, std::string s, void *param) {
    if (!suc) {
        return;
    }
    
    rapidjson::Document d;
    d.Parse<0>(s.c_str());
    inLogin = false;
    if(d["state"].GetInt() == 0) {
        CCLog("error infomation %s %x", d["err"].GetString(), error);
        error->setEnabled(true);
        error->runAction(CCSequence::create(CCScaleTo::create(0.1, 1.2, 1.2), CCScaleTo::create(0.1, 1, 1), NULL));
        error->setText(d["err"].GetString());
    }else {
        CCLog("登录成功 获得用户数据");
        Logic::getInstance()->setUID(d["uid"].GetInt());
        Logic *lg = Logic::getInstance();
        
        //lg->loginName = d["udata"]["loginName"].GetString();
        //lg->nickname = d["udata"]["nickname"].GetString();
        
        lg->storeData();
        CCDirector::sharedDirector()->replaceScene(WorldCup::scene());
    }
    
}
void LoginView::onReg(CCObject *obj, TouchEventType tt) {
    switch (tt) {
        case cocos2d::ui::TOUCH_EVENT_BEGAN:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_MOVED:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_ENDED:
        {
            CCDirector::sharedDirector()->replaceScene(RegisterView::scene());
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_CANCELED:
        {
            
        }
            break;
        default:
            break;
    }
}


void LoginView::update(float diff){
    if (!checkYet) {
        checkYet = true;
        
        CCUserDefault *u = CCUserDefault::sharedUserDefault();
        std::string uname = u->getStringForKey("loginName");
        
        //登录 或者注册
        if (uname.compare("") == 0) {
            
        //用户登录过 直接进入应用
        }else {
            
            //Logic::getInstance()->loginName = uname;
            Logic::getInstance()->setUID(u->getIntegerForKey("uid"));
            //Logic::getInstance()->nickname = u->getStringForKey("nickname");
            
            CCLog("该用户已经登陆过 进入比赛页面");
            CCDirector::sharedDirector()->replaceScene(WorldCup::scene());
            
        }
    }
}
