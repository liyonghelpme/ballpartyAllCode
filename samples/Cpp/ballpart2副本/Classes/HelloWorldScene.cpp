#include "HelloWorldScene.h"
#include "AppMacros.h"
#include "RedisInterface.h"
#include "cocos-ext.h"
#include "HttpModel.h"

USING_NS_CC;

using namespace ui;
using namespace extension;


CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    //connect();
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback));
    
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Arial", TITLE_FONT_SIZE);
    
    // position the label on the center of the screen
    pLabel->setPosition(ccp(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - pLabel->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(pLabel, 1);

    // add "HelloWorld" splash screen"
    CCSprite* pSprite = CCSprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    pSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);
    
    CCMenuItemImage *record = CCMenuItemImage::create("record.png", "record_h.png", this, menu_selector(HelloWorld::record));
    CCMenuItemImage *stop = CCMenuItemImage::create("stop.png", "stop_h.png", this, menu_selector(HelloWorld::stop));
    CCMenuItemImage *play = CCMenuItemImage::create("play.png", "play_h.png", this, menu_selector(HelloWorld::play));
    CCMenuItemLabel *send = CCMenuItemLabel::create(CCLabelTTF::create("send", "", 100), this, menu_selector(HelloWorld::send));
    //start receive thread
    CCMenuItemLabel *receive = CCMenuItemLabel::create(CCLabelTTF::create("receive", "", 100), this, menu_selector(HelloWorld::receive));
    CCMenuItemLabel *http = CCMenuItemLabel::create(CCLabelTTF::create("http", "", 100), this, menu_selector(HelloWorld::http));
    
                                                       
    
    CCMenu *menu = CCMenu::create(record, stop, play, send, receive, http, NULL);
    addChild(menu);
    menu->setPosition(ccp(300, 300));
    menu->setScale(0.25);
    menu->setPosition(ccp(0, 0));
    menu->alignItemsHorizontally();
    testR = NULL;
    /*
    TouchGroup *ui = TouchGroup::create();
    UIWidget* addshow = GUIReader::shareReader()->widgetFromJsonFile("RegisterScene.ExportJson");
	ui->addWidget(addshow);
	//addshow->setSize(DGloabData::shareGloabData()->m_winSize);
	ui->setAnchorPoint(CCPointZero);
	this->addChild(ui);
    */
    
    
    //test UIListView
    UILayer *lay = UILayer::create();
    addChild(lay);
    
    /*
    UIWidget *wid = GUIReader::shareReader()->widgetFromJsonFile("UITest/UITest.json");
    lay->addWidget(wid);
    
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    CCSize rootSize = wid->getSize();
    lay->setPosition(CCPoint((size.width-rootSize.width)/2, (size.height-rootSize.height)/2));
    
    CCLog("size is %f %f %f %f", size.width, size.height, rootSize.width, rootSize.height);
    
    
    
    Button *left_button = static_cast<Button*>(UIHelper::seekWidgetByName(wid, "left_Button"));
    left_button->addTouchEventListener(this, toucheventselector(HelloWorld::onLeft));
    
    
    UILabel *lab = UILabel::create();
    lab->setText("Test Lab");
    lab->setFontName("Marker Felt");
    lab->setFontSize(32);
    lab->setAnchorPoint(ccp(0.5, -1));
    lab->setPosition(ccp(rootSize.width/2, rootSize.height/2));
    lay->addWidget(lab);
    
    CCArray *arr = CCArray::create();
    CC_SAFE_RETAIN(arr);
    
    for(int i=0; i < 20; i++) {
        CCString *cs = CCString::createWithFormat("listItem %d", i);
        arr->addObject(cs);
    }
    
    CCSize frameSize = CCDirector::sharedDirector()->getWinSizeInPixels();
    CCLog("pixel Size %f %f", frameSize.width, frameSize.height);
     */
    
    /*
    UIListView *lv = UIListView::create();
    lv->setDirection(SCROLLVIEW_DIR_VERTICAL);
    lv->setTouchEnabled(true);
    lv->setBounceEnabled(true);
    //lv->setBackGroundColor(ccc3(102, 10, 20));
    lv->setSize(frameSize);
    lv->setPosition(ccp(frameSize.width/2, frameSize.height/2));
    lv->addEventListenerListView(this, listvieweventselector(HelloWorld::selectItem));
    lay->addWidget(lv);
    lv->setPosition(ccp(0, 0));
    lay->setPosition(ccp(0, 0));
    
    lv->setItemsMargin(10);
    
    UIButton *db = UIButton::create();
    db->setName("TestButton");
    db->setTouchEnabled(true);
    db->loadTextures("UITest/r1.png", "UITest/r2.png", "");
    
    
    Layout *di = Layout::create();
    di->setTouchEnabled(true);
    di->setSize(db->getSize());
    db->setPosition(ccp(di->getSize().width/2, di->getSize().height/2));
    di->addChild(db);
    lv->setItemModel(di);
    int count = arr->count();
    for(int i=0; i < count/4; i++) {
        lv->pushBackDefaultItem();
    }
    for (int i=0; i < count/4; i++) {
        lv->insertDefaultItem(1);
    }
    for(int i=0; i < count/4; i++){
        UIButton *cb = UIButton::create();
        cb->setName("custom button");
        cb->setTouchEnabled(true);
        cb->loadTextures("f1.png", "f2.png", "");
        cb->setScale9Enabled(true);
        cb->setSize(db->getSize());
        
        
        Layout *cl = Layout::create();
        cl->setSize(cb->getSize());
        cb->setPosition(ccp(cl->getSize().width/2, cl->getSize().height/2));
        cl->addChild(cb);
        
        lv->pushBackCustomItem(cl);
        
    }
    */
    /*
    CCArray *items = lv->getItems();
    int ic = items->count();
    for (int i=0; i < ic; i++) {
        
    }
    */
    
    /*
    Layout *title = Layout::create();
    lay->addWidget(title);
    title->setPositionPercent(ccp(0, 0.9));
    title->setAnchorPoint(ccp(0, 1));
    title->setSizePercent(ccp(1, 0.1));
    
    
    UILabel *tlab = Label::create();
    tlab->setText("世界杯");
    tlab->setFontName("Mark Felt");
    tlab->setFontSize(50);
    tlab->setColor(ccc3(255, 255, 255));
    title->addChild(tlab);
    tlab->setPositionPercent(ccp(0.5, 0.5));
    
    
    
    
    
    ListView *lv = ListView::create();
    lay->addWidget(lv);
    lv->setPosition(ccp(0, 0));
    lv->setSizePercent(ccp(1, 0.9));
    
    */
    
    Widget *w = GUIReader::shareReader()->widgetFromJsonFile("gui/ballUI_1.json");
    lay->addWidget(w);
    
    
    scheduleUpdate();
    return true;
}
void HelloWorld::selectItem(CCObject* obj, ListViewEventType type){
    
}
void HelloWorld::onLeft(CCObject *sender, TouchEventType type) {
    
}
void HelloWorld::update(float time) {
    //return;
    if(testR != NULL) {
        std::string channel;
        std::string content;
        bool res = readSubData(testR, &channel, &content);
        if(res){
            CCLog("read data %s %s", channel.c_str(), content.c_str());
        }
    }
}
void HelloWorld::http(cocos2d::CCObject *ps){
    HttpModel *hm = HttpModel::getInstance();
    std::map<std::string, std::string> postData;
    postData["loginName"] = "xiaoming";
    postData["password"] = "123345";
    hm->addRequest("login", "GET", postData, this, MYHTTP_SEL(HelloWorld::response), NULL);
    
    hm->addRequest("saveProfile", "POST", postData, this, MYHTTP_SEL(HelloWorld::response), NULL);
    
}

void HelloWorld::response(bool suc, std::string s, void*param){
    rapidjson::Document d;
    d.Parse<0>(s.c_str());
    CCLog("finish response");
    //printf("name is %s\n", d["loginName"].GetString());
    
    //printf("world response name %s\n", d["loginName"].GetString());
}
void HelloWorld::receive(cocos2d::CCObject *pSender){
    testR = createRedis();
    connectRedis(testR);
    runSubscribe(testR);
    
}
void HelloWorld::record(CCObject *pSender) {
    startRecord();
}
void HelloWorld::stop(CCObject *pSender){
    stopRecord();
}
void HelloWorld::play(CCObject *pSender){
    startPlay();
}
void HelloWorld::send(CCObject *pSender){
    string fn = getFileName();
    CCLog("file name %s", fn.c_str());
    //connect redis server
    connect();
    //send data to server
    //tartSend(fn);
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
#endif
}
