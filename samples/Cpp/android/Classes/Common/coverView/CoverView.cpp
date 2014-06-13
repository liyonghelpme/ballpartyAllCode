#include "CoverView.h"  
  
CoverView::CoverView()  
{  
  
}  
  
CoverView::~CoverView()  
{  
    CC_SAFE_RELEASE_NULL(cardArray);  
}  
  
CoverView* CoverView::create(CCRect swBox, CCSize slSize , float disDistance , float disScale)  
{  
    CoverView* cover = new CoverView();  
    if(cover && cover->init(swBox,slSize,disDistance,disScale))  
    {  
        cover->autorelease();  
        return cover;  
    }  
    CC_SAFE_DELETE(cover);  
    return NULL;  
}  
  
bool CoverView::init(CCRect swBox , CCSize slSize , float disDistance , float disScale)  
{  
    if(!CCNode::init()) return false;  
    this->swBox = swBox;  
    this->swPosition = swBox.origin;  
    this->swSize = swBox.size;  
    this->slSize = slSize;  
    this->disDistance = disDistance;  
    this->disScale = disScale;  
    initData();
    selectBg = CCSprite::create("img_chosenLI.png");
    addChild(selectBg, 2);
    //selectBg->setVisible(false);
    
    selectBg->setPosition(ccp(swBox.size.width/ 2, 101));
    return true;  
}  
  
  
void CoverView::initData()  
{  
    wSize = CCDirector::sharedDirector()->getWinSize();  
    cardArray = CCArray::create();  
    cardArray->retain();  
    cardNum = 0;  
  
    offsetPosition = ccp(swSize.width/2,swSize.height/2);  
  
    scrollLayer = CCLayer::create();  
    scrollLayer->setAnchorPoint(CCPointZero);  
    scrollLayer->setPosition(CCPointZero);  
    scrollLayer->setContentSize(slSize);  
    scrollView = CCScrollView::create(swSize,scrollLayer);  
    scrollView->setAnchorPoint(CCPointZero);  
    scrollView->setContentOffset(ccp(0,0));  
    scrollView->setTouchEnabled(false);    
    scrollView->setDelegate(this);    
    scrollView->setDirection(kCCScrollViewDirectionHorizontal);    
    addChild(scrollView,1);  
}  
  
void CoverView::onEnter()  
{  
    CCNode::onEnter();  
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0 , false);    
}  
  
void CoverView::onExit()  
{  
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);  
    removeAllChildren();  
    CCNode::onExit();  
}  
  
bool CoverView::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)  
{
    return true;  
}  
  
void CoverView::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)  
{  
    CCPoint scroll_prepoint = pTouch->getPreviousLocation();  
    CCPoint scroll_movepoint = pTouch->getLocation();  
    if(swBox.containsPoint(scroll_movepoint))  
    {
        selectBg->setVisible(false);
        CCPoint adjustPoint = scroll_movepoint-scroll_prepoint;  
        adjustScrollView(adjustPoint);    
        adjustCardScale(adjustPoint);  
    }  
}  
  
void CoverView::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)  
{  
    CCPoint scroll_prepoint = pTouch->getPreviousLocation();  
    CCPoint scroll_endpoint = pTouch->getLocation();  
    float disX = scroll_endpoint.x - scroll_endpoint.x;  
    adjusetEndScrollView();
    if(!selectBg->isVisible()){
        CCFadeIn* fadeIn = CCFadeIn::create(0.2f);
        selectBg->runAction(fadeIn);
        selectBg->setVisible(true);
    }
}
  
void CoverView::adjustCardScale(CCPoint adjustPoint)  
{  
    float disX = adjustPoint.x;  
    CCObject* obj = NULL;  
    CCARRAY_FOREACH(cardArray,obj)  
    {  
        CCSprite* card = (CCSprite*) obj;
        float offset = scrollView->getContentOffset().x;  
        float posX = card->getPositionX() + offset;  
        float disMid = abs(swSize.width/2-posX);  
        float scale = 1- disMid/disDistance*disScale;  
        card->setScale(scale);
        //设置透明度
        card->setOpacity(100);
        CCArray* tmp = card->getChildren();
        CCSprite* tmpCard = (CCSprite*)tmp->objectAtIndex(0);
        tmpCard->setOpacity(100);
        //设置字体
        UILabel* mfont = (UILabel*)tmp->objectAtIndex(1);
        mfont->setEnabled(true);
        
        int zOr = (int) (1000-disMid*0.1);  
        card->setZOrder(zOr);  
    }  
}  
  
void CoverView::adjustScrollView(CCPoint adjustPoint)  
{  
    CCPoint endPoint = ccpAdd(scrollView->getContentOffset(),ccp(adjustPoint.x,0));  
    scrollView->unscheduleAllSelectors();  
    scrollView->setContentOffset(endPoint,false);  
}  
  
void CoverView::adjusetEndScrollView()  
{  
    CCObject* obj = NULL;  
    float minX = wSize.height;  
    float midX = swSize.width/2;  
    //获取距离中间最小值的card  
    CCARRAY_FOREACH(cardArray,obj)  
    {  
        CCNode* card = (CCNode*) obj;  
        float offset = scrollView->getContentOffset().x;  
        //转化父类坐标  
        float posX = card->getPositionX() + offset;  
        float disMid = midX-posX;  
        if(abs(disMid) < abs(minX)) minX = disMid;  
    }  
  
    CCARRAY_FOREACH(cardArray,obj)  
    {  
        CCSprite* item = (CCSprite*) obj;
        //转化父类坐标  
        float offset = scrollView->getContentOffset().x;  
        float posX = item->getPositionX() + offset ;  
        //距离中间长度  
        float disMid = abs(midX - posX - minX);  
        //目标scale  
        float scale = 1- disMid/disDistance*disScale;  
        CCScaleTo* scaleBy = CCScaleTo::create(0.2f,scale);
        item->runAction(scaleBy);
        //设置透明度
        float opcity = abs(scale)*255;
        if(opcity < 100){ opcity = 100;}
        if(opcity > 255){ opcity = 255;}
        item->setOpacity(opcity);
        
        CCArray* tmp = item->getChildren();
        CCSprite* tmpCard = (CCSprite*)tmp->objectAtIndex(0);
        tmpCard->setOpacity(opcity);
        
        int zOr = (int) (1000-disMid*0.1);  
        item->setZOrder(zOr);  
    }  
    CCLayer* scrollLayer = (CCLayer*)scrollView->getContainer();  
    CCMoveBy* moveBy = CCMoveBy::create(0.2f,ccp(minX,0));  
    //CCCallFuncN* callFuncN = CCCallFuncN::create(this,callfuncN_selector(CoverView::cardViewEnd_callBack));  
    //CCSequence* seq = CCSequence::create(moveBy,callFuncN,NULL);  
    //scrollLayer->runAction(seq);  
    scrollLayer->runAction(moveBy);
    
}  
  
void CoverView::cardViewEnd_callBack(CCNode* pSender)  
{  
    getCurCardIndex();  
}  
  
void CoverView::scrollViewDidScroll(CCScrollView* view)  
{  
  
}  
  
void CoverView::scrollViewDidZoom(CCScrollView* view)  
{  
  
}  
  
  
  
void CoverView::addCard(CCNode * card)  
{
    int zOrder = 1000 - cardNum;  
    this->addCard(card, zOrder, 0);  
}  
  
void CoverView::addCard(CCNode * card, int zOrder)  
{  
    this->addCard(card, zOrder,0);  
}  
  
void CoverView::addCard(CCNode* card, int zOrder, int tag)  
{  
    float positionX = offsetPosition.x + disDistance*cardNum;  
    float scale = 1 - disScale*cardNum;  
    card->setPosition(ccp(positionX,offsetPosition.y));  
    card->setScale(scale);  
    cardArray->addObject(card);  
    scrollLayer->addChild(card , zOrder,tag);  
    cardNum++;  
    //CCLog("crad%d:[%f , %f]",cardNum,card->getPositionX(),card->getPositionY());  
}  
  
int CoverView::getCurCardIndex()  
{  
    float distance1 = - scrollLayer->getPositionX();  
    float distance2 = swSize.width/2 - offsetPosition.x;  
    //+5 浮点数误差  
    int index = (distance1 + distance2 + 5) / (disDistance);  
    //CCLog("card index:%d  distance1:%f",index,distance1);  
    return index;  
}  
  
void CoverView::setOffsetPosition(CCPoint var)  
{  
    offsetPosition = var;  
    CCObject* obj = NULL;  
    cardNum = 0;  
    CCARRAY_FOREACH(cardArray,obj)  
    {  
        CCNode* card = (CCNode*) obj;  
        float positionX = offsetPosition.x + disDistance*cardNum;  
        card->setPosition(ccp(positionX,offsetPosition.y + 50));
        cardNum++;  
    }  
    adjustCardScale(CCPointZero);  
}  
  
CCPoint CoverView::getOffsetPosition()  
{  
    return offsetPosition;  
}