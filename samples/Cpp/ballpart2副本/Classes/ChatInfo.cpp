//
//  ChatInfo.cpp
//  TestCpp
//
//  Created by dozengame on 14-5-19.
//
//

#include "ChatInfo.h"
#include "Logic.h"
#include "User.h"

CCScene *ChatInfo::scene(){
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    ChatInfo *layer = ChatInfo::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

bool ChatInfo::init(){
    if ( !CCLayer::init() )
    {
        return false;
    }
    getYet = false;
    
    
    
    CCSize size = CCDirector::sharedDirector()->getVisibleSize();
    CCLog("winSize %f %f", size.width, size.height);
    
    UILayer *lay = UILayer::create();
    addChild(lay);

    w = GUIReader::shareReader()->widgetFromJsonFile("gui/ballUI_8_0.json");
    lay->addWidget(w);
    w->setSize(size);
    
    row = static_cast<Layout*>(UIHelper::seekWidgetByName(w, "row"));
    row->setEnabled(false);
    
    lv = static_cast<ListView*>(UIHelper::seekWidgetByName(w, "ListView_5"));
    
    /*
    for (int i=0; i < 5; i++) {
        Layout* rc = (Layout*)row->clone();
        rc->setEnabled(true);
        lv->pushBackCustomItem(rc);
    }
    */
    
    Button *back = static_cast<Button*>(UIHelper::seekWidgetByName(w, "back"));
    back->addTouchEventListener(this, toucheventselector(ChatInfo::onBack));
    
    
    scheduleUpdate();
    return true;
}
void ChatInfo::onBack(CCObject*obj, TouchEventType tt){
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

//登录房间的时候 应该附带有用户的基本讯息
void ChatInfo::update(float dt){
    //获取房间信息
    if (!getYet) {
        if (!Logic::getInstance()->chatInfoYet) {
            Logic::getInstance()->initChatInfo();
        } else if (Logic::getInstance()->initChatYet){
            getYet = true;
            
            //获取了房间用户信息之后 下次进入可以再次请求
            //清理获取的网络状态
            Logic::getInstance()->chatInfoYet = false;
            Logic::getInstance()->initChatYet = false;
            
            rapidjson::Document &d = Logic::getInstance()->chatD;
            rapidjson::Value &b = d["data"];
            Layout *rc;
            //Button *b1, *b2, *b3;
            Layout *l1, *l2, *l3;
            Label *online = static_cast<Label*>(UIHelper::seekWidgetByName(w, "online"));
            char buf[512];
            sprintf(buf, "在线人数(%d)", b.Size());
            online->setText(buf);
            
            
            char bf1[256];
            User user;
            for (int i =0; i < b.Size(); i++) {
                user.setChatInfo(b[i]);
                
                if (i%3 == 0) {
                    rc = (Layout*)row->clone();
                    rc->setEnabled(true);
                    rc->setVisible(true);
                    
                    lv->pushBackCustomItem(rc);
                    l1 = static_cast<Layout*>(UIHelper::seekWidgetByName(rc, "p1"));
                    l2 = static_cast<Layout*>(UIHelper::seekWidgetByName(rc, "p2"));
                    l3 = static_cast<Layout*>(UIHelper::seekWidgetByName(rc, "p3"));
                    
                    Label *name = static_cast<Label*>(UIHelper::seekWidgetByName(l1, "name1"));
                    name->setText(user.realName);
                    l2->setEnabled(false);
                    l3->setEnabled(false);
                    
                    sprintf(bf1, "flags/%d.png", user.flagId);
                    Button *but = static_cast<Button*>(UIHelper::seekWidgetByName(l1, "Button_7"));
                    but->loadTextureNormal(bf1);
                    
                    
                    //b2->setEnabled(false);
                    //b3->setEnabled(false);
                } else if(i % 3 == 1) {
                    //b2->setEnabled(true);
                    l2->setEnabled(true);
                    Label *name = static_cast<Label*>(UIHelper::seekWidgetByName(l2, "name1"));
                    name->setText(user.realName);
                    
                    
                    sprintf(bf1, "flags/%d.png", user.flagId);
                    Button *but = static_cast<Button*>(UIHelper::seekWidgetByName(l2, "Button_7"));
                    but->loadTextureNormal(bf1);
                }  else {
                    l3->setEnabled(true);
                    //b3->setEnabled(true);
                    Label *name = static_cast<Label*>(UIHelper::seekWidgetByName(l3, "name1"));
                    name->setText(user.realName);
                    
                    sprintf(bf1, "flags/%d.png", user.flagId);
                    Button *but = static_cast<Button*>(UIHelper::seekWidgetByName(l3, "Button_7"));
                    but->loadTextureNormal(bf1);
                }
            }
            
        }
    }
}


