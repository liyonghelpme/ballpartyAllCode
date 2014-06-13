#include "Loading.h"
#include "LoginScene.h"

CCScene *Loading::scene(){
    CCScene *scene = CCScene::create();

    // 'layer' is an autorelease object
    Loading *layer = Loading::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

void Loading::update(float diff) {
	passTime += diff;
	CCLog("pass time %f", passTime);
	if(passTime >= 2) {
		CCDirector::sharedDirector()->replaceScene(LoginScene::scene());
	}
}


bool Loading::init(){
	if ( !CCLayer::init() )
    {
        return false;
    }
    passTime = 0;

    CCSprite *sp = CCSprite::create("loadPage.png");
    CCSize vs = CCDirector::sharedDirector()->getVisibleSize();

    sp->setPosition(ccp(vs.width/2, vs.height/2));
    addChild(sp);


    scheduleUpdate();


    return true;
}