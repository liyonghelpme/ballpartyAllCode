//
//  FinishReg.cpp
//  TestCpp
//
//  Created by dozengame on 14-5-19.
//
//

#include "FinishReg.h"
#include "HttpModel.h"
#include "Logic.h"
#include "WorldCup.h"
#include "RedisInterface.h"
#include "MyBase64.h"



CCScene *FinishReg::scene(){
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    FinishReg *layer = FinishReg::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

bool FinishReg::init() {
    if ( !CCLayer::init() )
    {
        return false;
    }
    inReg = false;
    setHeadYet = false;
    
    
    CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
    CCSize size = CCDirector::sharedDirector()->getVisibleSize();
    UILayer *lay = UILayer::create();
    addChild(lay);

    
    w = GUIReader::shareReader()->widgetFromJsonFile("gui/ballUI_6.json");
    lay->addWidget(w);
    w->setSize(size);
    
    finBut = static_cast<Button*>(UIHelper::seekWidgetByName(w, "finishRegister"));
    finBut->addTouchEventListener(this, toucheventselector(FinishReg::onFin));
    
    name = static_cast<TextField*>(UIHelper::seekWidgetByName(w, "name"));
    error = static_cast<Label*>(UIHelper::seekWidgetByName(w, "error"));
    error->setEnabled(false);
    
    head = static_cast<Button*>(UIHelper::seekWidgetByName(w, "head"));
    head->addTouchEventListener(this, toucheventselector(FinishReg::onHead));
    
    
    netLay = static_cast<Layout*>(UIHelper::seekWidgetByName(w, "Panel_8"));
    netLay->setEnabled(false);
    
    
    scheduleUpdate();
    return true;
}
void FinishReg::onHead(CCObject *obj, TouchEventType tt) {
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
            //当前未设置头像 重新获取头像数据
            setHeadYet = false;
            openImageSelect();
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

void FinishReg::onFin(cocos2d::CCObject *obj, TouchEventType tt){
    
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
            if (inReg) {
                return;
            }
            string n = name->getStringValue();
            if (n.length() == 0) {
                error->setEnabled(true);
                error->setText("昵称不合法");
                error->runAction(CCSequence::create(CCScaleTo::create(0.1, 1.2, 1.2), CCScaleTo::create(0.1, 1), NULL));
                return;
            }
            
            
            inReg = true;
            HttpModel *hm = HttpModel::getInstance();
            std::map<string, string>pd;
            char uid[128];
            //sprintf(uid, "%d", Logic::getInstance()->getUID());
            pd["uid"] = uid;
            pd["nickname"] = n;
            netLay->setEnabled(true);
            
            //base64 编码的头像数据
            int len;
            void *data = getImage(&len);
            if (data != NULL) {
                int olen;
                char *b64 = base64_encode((unsigned char*)data, (size_t)len, (size_t *)&olen);
                string hdata = string(b64, olen);
                pd["head"] = hdata;
                free(b64);
                //free ImageData  _uimg
                //dealloc sharedImageSelect
            }
            
            hm->addRequest("user/finReg", "POST", pd, this, MYHTTP_SEL(FinishReg::registerOver), NULL);
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

void FinishReg::registerOver(bool suc, std::string s, void *param) {
    if (!suc) {
        return;
    }
    rapidjson::Document d;
    d.Parse<0>(s.c_str());
    if(d["state"].GetInt() == 0){
        
    }else {
        //下次不用注册自动登录即可
        string n = name->getStringValue();
        //Logic::getInstance()->nickname = n;
        
        
        CCUserDefault *u = CCUserDefault::sharedUserDefault();
        
        //u->setStringForKey("loginName", Logic::getInstance()->loginName);
        //u->setIntegerForKey("uid", Logic::getInstance()->getUID());
        //u->setStringForKey("nickname", Logic::getInstance()->nickname);
        
        
        
        CCDirector::sharedDirector()->replaceScene(WorldCup::scene());
    }
}

//可能用户取消了 选择图片
//可能用户 换了一张图片
//用户使用相同的 图片
void FinishReg::update(float df){
    bool cg = checkGetYet();
    //CCLog("set HeadYet %d %d", setHeadYet, cg);
    if (!setHeadYet && cg) {
        setHeadYet = true;
        
        int len;
        void *data = getImage(&len);
        CCLog("select Img %x %d", data, len);
        
        if (data != NULL) {
            const char *key = "_uimg";
            CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->textureForKey(key);
            CCImage *pImage = new CCImage();
            pImage->initWithImageData(data, len, CCImage::kFmtPng);
            pTexture = CCTextureCache::sharedTextureCache()->addUIImage(pImage, key);
            CC_SAFE_RELEASE(pImage);
            
            head->loadTextureNormal(key, UI_TEX_TYPE_LOCAL);
        }
    }
}