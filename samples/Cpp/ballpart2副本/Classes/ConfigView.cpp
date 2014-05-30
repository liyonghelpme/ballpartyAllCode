//
//  ConfigView.cpp
//  TestCpp
//
//  Created by dozengame on 14-5-19.
//
//

#include "ConfigView.h"
#include "Logic.h"
#include "LoginScene.h"
#include "ServiceCenter.h"

CCScene *ConfigView::scene(){
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    ConfigView *layer = ConfigView::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

bool ConfigView::init(){
    if ( !CCLayer::init() )
    {
        return false;
    }
    getYet = false;
    
    CCSize size = CCDirector::sharedDirector()->getVisibleSize();
    CCLog("winSize %f %f", size.width, size.height);
    
    UILayer *lay = UILayer::create();
    addChild(lay);
    //lay->setPosition(ccp(size.width/2, size.height/2));
    //lay->setContentSize(size);
    
    
    
    w = GUIReader::shareReader()->widgetFromJsonFile("gui/ballUI_7_0.json");
    lay->addWidget(w);
    w->setSize(size);

    Button *back = static_cast<Button*>(UIHelper::seekWidgetByName(w, "back"));
    back->addTouchEventListener(this, toucheventselector(ConfigView::onBack));
    
    Button *loginOut = static_cast<Button*>(UIHelper::seekWidgetByName(w, "quit"));
    loginOut->addTouchEventListener(this, toucheventselector(ConfigView::onQuit));
    
    scheduleUpdate();
    return true;
}

void ConfigView::initView() {
    CCLog("init View");
    Button *head = static_cast<Button*>(UIHelper::seekWidgetByName(w, "head"));
    char buf[512];
    sprintf(buf, "flags/%d.png", Logic::getInstance()->getFlagId());
    head->loadTextureNormal(buf);
    adjustScale(head, 70, 70);
    
    UserService *us = (UserService*)ServiceCenter::getInstance()->getService(ServiceCenter::USER_SERVICE);
    User *user = us->getUser();
    
    TextField  *nn = static_cast<TextField*>(UIHelper::seekWidgetByName(w, "nickname"));
    nn->setTouchEnabled(false);
    nn->setText(user->realName);
    
    TextField *account = static_cast<TextField*>(UIHelper::seekWidgetByName(w, "account"));
    account->setTouchEnabled(false);
    
    account->setText(user->phoneNumber);
    
    TextField *team = static_cast<TextField*>(UIHelper::seekWidgetByName(w, "team"));
    team->setTouchEnabled(false);
    team->setText(user->referrer_nick);
//=======
//    team->setText(Logic::getInstance()->getReferrer());
//>>>>>>> .r36322
    
    
    TextField *gender = static_cast<TextField*>(UIHelper::seekWidgetByName(w, "gender"));
    gender->setTouchEnabled(false);
    gender->setText(user->getGender());
    
    
    TextField *local = static_cast<TextField*>(UIHelper::seekWidgetByName(w, "local"));
    local->setTouchEnabled(false);
    local->setText(user->area);
}

void ConfigView::onBack(cocos2d::CCObject *obj, TouchEventType tt){
    
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
            CCDirector::sharedDirector()->popSceneWithTransition(transTime);
            curInScene = NULL;
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

void ConfigView::onQuit(cocos2d::CCObject *obj, TouchEventType tt){
    
    switch (tt) {
        case cocos2d::ui::TOUCH_EVENT_BEGAN:
        {
            
        }
            break;
        case cocos2d::ui::TOUCH_EVENT_MOVED:
        {
            
        }
            break;
        //但是内部的 WorldCup Scene 也要抛弃掉
        case cocos2d::ui::TOUCH_EVENT_ENDED:
        {
            CCDirector* pDirector = CCDirector::sharedDirector();
            CCScene* pScene = LoginScene::scene();
            //pDirector->replaceScene(getTransScene(pScene));
            //pDirector->replaceRootScene(getTransScene(pScene));
            pDirector->popToRootScene();
            //pDirector->replaceScene(getTransScene(pScene));
            //Transition 场景存在 问题 一旦Transition 就会导致 touch无效 应该是 touch被关闭掉了 但是没有开启起来 TransitionScene 处理的代码问题
            pDirector->replaceScene(pScene);
            
            //退出登录清理状态
            curInScene = NULL;
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

void ConfigView::update(float dt){
    if (!getYet) {
        //getYet = true;
        if (Logic::getInstance()->fetchInfoState == 0) {
            Logic::getInstance()->fetchInfo();
        } else if(Logic::getInstance()->fetchInfoState == 2) {
            getYet = true;
            initView();
            
        }
    }
    
}
