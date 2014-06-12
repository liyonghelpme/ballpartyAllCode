//
//  FriendView.cpp
//  TestCpp
//
//  Created by dozengame on 14-5-19.
//
//

#include "FriendView.h"

CCScene *FriendView::scene(){
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    FriendView *layer = FriendView::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

bool FriendView::init(){
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize size = CCDirector::sharedDirector()->getVisibleSize();
    CCLog("winSize %f %f", size.width, size.height);
    
    UILayer *lay = UILayer::create();
    addChild(lay);
    //lay->setPosition(ccp(size.width/2, size.height/2));
    //lay->setContentSize(size);
    
    
    
    w = GUIReader::shareReader()->widgetFromJsonFile("gui/ballUI_9.json");
    lay->addWidget(w);
    w->setSize(size);
    
    Button *back = static_cast<Button*>(UIHelper::seekWidgetByName(w, "Button_4"));
    back->addTouchEventListener(this, toucheventselector(FriendView::onBack));
    
    lv = static_cast<ListView*>(UIHelper::seekWidgetByName(w, "ListView_5"));
    //lv->setEnable(false);
    lv->setEnabled(true);
    lv->setVisible(true);
    
    
    alpha = static_cast<Layout*>(UIHelper::seekWidgetByName(w, "alphaLabel"));
    alpha->setEnabled(false);
    people = static_cast<Layout*>(UIHelper::seekWidgetByName(w, "people"));
    people->setEnabled(false);
    
    for (int i = 0; i < 50; i++) {
        Layout *ac = (Layout*)alpha->clone();
        ac->setEnabled(true);
        ac->setVisible(true);
        Layout *pc = (Layout*)people->clone();
        pc->setEnabled(true);
        pc->setVisible(true);
        lv->pushBackCustomItem(ac);
        lv->pushBackCustomItem(pc);
    }
    
    return true;
}


void FriendView::onBack(cocos2d::CCObject *obj, TouchEventType tt){
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
            CCDirector::sharedDirector()->popScene();
            
            CCLog("back btn click!");
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

void FriendView::update(float dt){
    
}