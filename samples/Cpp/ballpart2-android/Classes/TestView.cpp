//
//  TestView.cpp
//  ballparty
//
//  Created by dozengame on 14-5-22.
//
//

#include "TestView.h"
CCScene *TestView::scene(){
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    TestView *layer = TestView::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

bool TestView::init(){
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize size = CCDirector::sharedDirector()->getVisibleSize();
    CCLog("winSize %f %f", size.width, size.height);
    
    UILayer *lay = UILayer::create();
    addChild(lay);
    
    Widget *w = GUIReader::shareReader()->widgetFromJsonFile("testgui/LoginSence.json");
    lay->addWidget(w);
    w->setSizeType(SIZE_ABSOLUTE);
    w->setSize(size);
    
    return true;
}

