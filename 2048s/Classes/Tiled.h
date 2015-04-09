#ifndef _Tiled_H_
#define _Tiled_H_
#include "cocos2d.h"

USING_NS_CC;

class Tiled:public Node{
public:
	int level;
	Sprite *backround;
	Label *label;
	static const int nums[16];
public:
	Tiled();
	virtual bool init();
	CREATE_FUNC(Tiled);
	void setLevel(int l);
};
#endif