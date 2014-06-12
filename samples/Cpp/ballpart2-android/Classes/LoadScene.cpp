//
//  LoadScene.cpp
//  ballparty
//
//  Created by dozengame on 14-5-29.
//
//

#include "LoadScene.h"

CCScene *LoadScene::scene(){
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    LoadScene *layer = LoadScene::create();
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}
bool LoadScene::init(){
    if ( !CCLayer::init() )
    {
        return false;
    }
    CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
    CCSprite *sp = CCSprite::create("loadPage.png");
    addChild(sp);
    sp->setPosition(CCSizeMake(fs.width/2, fs.height/2));
    return true;
}