#ifndef _LoadingScene_H_
#define _LoadingScene_H_
#include "cocos2d.h"

USING_NS_CC;

class LoadingScene:public Scene{
public:
	CREATE_FUNC(LoadingScene);
	virtual bool init();
	void onEnter() override;
	void loadComplete(Texture2D *texture);
};
#endif