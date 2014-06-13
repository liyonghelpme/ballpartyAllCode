#ifndef __LOADING__
#define __LOADING__
#include "cocos2d.h"

using namespace cocos2d;

class Loading : public CCLayer{
public:
	static CCScene *scene();
	CREATE_FUNC(Loading);

	virtual bool init();
	virtual void update(float);

private:
	float passTime;

};
#endif