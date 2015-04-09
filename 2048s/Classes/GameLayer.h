#ifndef _GameLayer_H_
#define _GameLayer_H_
#include "cocos2d.h"
#include "Tiled.h"

USING_NS_CC;
using namespace std;

class GameLayer:public Layer{
public:
	virtual bool init();
	CREATE_FUNC(GameLayer);
	GameLayer();
	void gameInit();
	Point touchDown;
	static int score;
	Label *lScore;
private:
	Tiled* tables[4][4];

	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);

	//四个移动方向，返回是否有砖块移动过
	bool moveToTop();
	bool moveToDown();
	bool moveToLeft();
	bool moveToRight();

	void swapTiled(Tiled *tiled1,Tiled * tiled2);
	void addTiled();
	bool isOver();
};
#endif