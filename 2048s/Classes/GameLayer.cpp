#include "GameLayer.h"
#include "Tiled.h"
#include "OverScene.h"
#include <time.h>
#include <random>
#include "GameScene.h"
#define RC_CONVERT_TO_XY(rc) (rc*105+60)

int GameLayer::score=0;

bool GameLayer::init(){
	bool bRet=false;
	do{
		CC_BREAK_IF(!Layer::init());

		auto cache=SpriteFrameCache::getInstance();
		Size size=Director::getInstance()->getVisibleSize();

		//添加背景
		auto background=Sprite::createWithSpriteFrame(cache->spriteFrameByName("background.png"));
		background->setPosition(Point(size.width/2,size.height/2));
		this->addChild(background);

		//添加标题背景
		auto headBg=Sprite::createWithSpriteFrame(cache->spriteFrameByName("title_bg.png"));
		headBg->setAnchorPoint(Point(0,1));
		headBg->setPosition(Point(0,640));
		this->addChild(headBg,1);

		//添加退出和重新开始按钮
		auto exitItem=MenuItemSprite::create(Sprite::createWithSpriteFrame(cache->spriteFrameByName("exit_norm.png")),
			Sprite::createWithSpriteFrame(cache->spriteFrameByName("exit_press.png")),NULL,[](Ref *psender){
#if(CC_TARGET_PLATFORM==CC_PLATFORM_WP8||CC_TARGET_PLATFORM==CC_PLATFORM_WINRT)
			MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
			return;
#endif
			Director::getInstance()->end();

#if(CC_TARGET_PLATFORM==CC_PLATFORM_IOS)
			exit(0);
#endif
		});
		exitItem->setPosition(Point(65,600));

		auto restartItem=MenuItemSprite::create(Sprite::createWithSpriteFrame(cache->spriteFrameByName("restart_norm.png")),
			Sprite::createWithSpriteFrame(cache->spriteFrameByName("restart_press.png")),NULL,[=](Ref *psender){
 			Director::getInstance()->replaceScene(GameScene::create());
 		});
 		restartItem->setPosition(Point(375,600));

		auto menu=Menu::create(exitItem,restartItem,NULL);
		menu->setAnchorPoint(Point::ZERO);
		menu->setPosition(Point::ZERO);
		this->addChild(menu,2);

		//添加砖块部分背景
		auto gameBg=Sprite::createWithSpriteFrame(cache->spriteFrameByName("game_bg.png"));
		gameBg->setAnchorPoint(Point::ZERO);
		gameBg->setPosition(Point(5,5));
		this->addChild(gameBg);

		//添加分数背景
		auto scoreBg=Sprite::createWithSpriteFrame(cache->spriteFrameByName("score_bg.png"));
		scoreBg->setAnchorPoint(Point::ZERO);
		scoreBg->setPosition(Point(5,435));
		this->addChild(scoreBg);

		//添加分数显示
		lScore=Label::create("0","Arial",40);
		lScore->setPosition(Point(size.width/4,470));
		this->addChild(lScore);

		//添加最高分显示
		int high=UserDefault::getInstance()->getIntegerForKey("HighScore",0);
		auto hScore=Label::create(String::createWithFormat("%d",high)->getCString(),"Arial",40);
		hScore->setPosition(Point(size.width/4*3,470));
		this->addChild(hScore);
		//初始化游戏界面
		gameInit();
		
		//添加监听器
		auto listener=EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan=CC_CALLBACK_2(GameLayer::onTouchBegan,this);
		listener->onTouchMoved=CC_CALLBACK_2(GameLayer::onTouchMoved,this);
		listener->onTouchEnded=CC_CALLBACK_2(GameLayer::onTouchEnded,this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,this);

		bRet=true;
	}while(0);
	return bRet;
}

void GameLayer::gameInit(){
	GameLayer::score=0;
	auto cache=SpriteFrameCache::getInstance();
	//初始化砖块
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			auto tiled=Tiled::create();
			tiled->level=0;
			tiled->setAnchorPoint(Point::ZERO);
			tiled->setPosition(Point(RC_CONVERT_TO_XY(j),RC_CONVERT_TO_XY(i)));
			tiled->setVisible(false);
			this->addChild(tiled,1);
			tables[i][j]=tiled;

		}
	}

	//获取两个随机坐标
	//c++11的随机数产生方式
	default_random_engine e(time(NULL));
	//这里是设定产生的随机数的范围，这里是0到3
	uniform_int_distribution<unsigned> u(0,3);
	int row1=u(e);
	int col1=u(e);
	int row2=u(e);
	int col2=u(e);
	//这个循环是保证两个砖块的坐标不会重复
	do{
		row2=u(e);
		col2=u(e);
	}while(row1==row2&&col1==col2);

	//添加第一个砖块
	auto tiled1=tables[row1][col1];
	int isFour=e()%10;
	if(isFour==0){
		tiled1->level=2;
		tiled1->backround->setDisplayFrame(cache->spriteFrameByName(String::createWithFormat("level2.png")->getCString()));
		tiled1->label->setString("4");
		tiled1->setVisible(true);
	}else{
		tiled1->level=1;
		tiled1->backround->setDisplayFrame(cache->spriteFrameByName(String::createWithFormat("level1.png")->getCString()));
		tiled1->label->setString("2");
		tiled1->setVisible(true);
	}

	//添加第二个砖块
	auto tiled2=tables[row2][col2];
	isFour=e()%10;
	if(isFour==0){
		tiled2->level=2;
		tiled2->backround->setDisplayFrame(cache->spriteFrameByName(String::createWithFormat("level2.png")->getCString()));
		tiled2->label->setString("4");
		tiled2->setVisible(true);
	}else{
		tiled2->level=1;
		tiled2->backround->setDisplayFrame(cache->spriteFrameByName(String::createWithFormat("level1.png")->getCString()));
		tiled2->label->setString("2");
		tiled2->setVisible(true);
	}
}


bool GameLayer::onTouchBegan(Touch *touch, Event *unused_event){
	this->touchDown=touch->getLocationInView();
	this->touchDown=Director::getInstance()->convertToGL(this->touchDown);
	return true;
}

void GameLayer::onTouchMoved(Touch *touch, Event *unused_event){

}

void GameLayer::onTouchEnded(Touch *touch, Event *unused_event){
	bool hasMoved=false;
	Point touchUp=touch->getLocationInView();
	touchUp=Director::getInstance()->convertToGL(touchUp);

	if(touchUp.getDistance(touchDown)>50){
		//判断上下还是左右
		if(abs(touchUp.x-touchDown.x)>abs(touchUp.y-touchDown.y)){
			//左右滑动
			if(touchUp.x-touchDown.x>0){
				//向右
				log("toRight");
				hasMoved=moveToRight();
			}else{
				//向左
				log("toLeft");
				hasMoved=moveToLeft();
			}
		}else{
			//上下滑动
			if(touchUp.y-touchDown.y>0){
				//向上
				log("toTop");
				hasMoved=moveToTop();
			}else{
				//向下
				log("toDown");
				hasMoved=moveToDown();
			}
		}

		if(hasMoved){
			addTiled();
		}

		if(isOver()){
			//存放分数
			int high=UserDefault::getInstance()->getIntegerForKey("HighScore",0);
			if(GameLayer::score>high){
				UserDefault::getInstance()->setIntegerForKey("HighScore",GameLayer::score);
				UserDefault::getInstance()->flush();
			}
			GameLayer::score=0;
			//切换画面
			Director::getInstance()->replaceScene(TransitionSlideInB::create(1.0f,OverScene::createScene()));
		}

	}
}

//四个方向
bool GameLayer::moveToDown(){
	bool hasMoved=false;
	//将数字相同的格子合一
 	for(int col=0;col<4;col++){
 		for(int row=0;row<4;row++){
			//遍历的每一次获得的方块
 			auto tiled=tables[row][col];
			//找到不为空的方块
 			if(tiled->level!=0){
 				int k=row+1;
				//看这一列有没有等级和这个方块等级相同的
 				while(k<4){
 					auto nextTiled=tables[k][col];
 					if(nextTiled->level!=0){
 						if(tiled->level==nextTiled->level){
							//找到等级和这个砖块等级相同的就把他们合并
  							tiled->setLevel(nextTiled->level+1);
 							nextTiled->setLevel(0);
 							nextTiled->setVisible(false);
 							GameLayer::score+=Tiled::nums[tiled->level];
 							this->lScore->setString(String::createWithFormat("%d",GameLayer::score)->getCString());
							hasMoved=true;
 						}
 						k=4;
 					}
 					k++;
 				}
 			}
 		}
 	}


	//将有数的格子填入空格子
	for(int col=0;col<4;col++){
		for(int row=0;row<4;row++){
			//遍历每一次的砖块
			auto tiled=tables[row][col];
			//找到空格子
			if(tiled->level==0){
				int k=row+1;
				while(k<4){
					auto nextTiled=tables[k][col];
					if(nextTiled->level!=0){
						//将不为空的格子移到这里
						tiled->setLevel(nextTiled->level);
						nextTiled->setLevel(0);
						tiled->setVisible(true);
						nextTiled->setVisible(false);
						hasMoved=true;
						k=4;
					}
					k++;
				}
			}
		}
	}
	
	return hasMoved;
}

//向左
bool GameLayer::moveToLeft(){ 
	bool hasMoved=false;
	//合成
 	for(int col=0;col<4;col++){
 		for(int row=0;row<4;row++){
 			auto tiled=tables[row][col];
 			if(tiled->level!=0){
 				int k=col+1;
 				while(k<4){
 					auto nextTiled=tables[row][k];
 					if(nextTiled->level!=0){
 						if(tiled->level==nextTiled->level){
							tiled->setLevel(nextTiled->level+1);
							nextTiled->setLevel(0);
							nextTiled->setVisible(false);
							GameLayer::score+=Tiled::nums[tiled->level];
							this->lScore->setString(String::createWithFormat("%d",GameLayer::score)->getCString());
							hasMoved=true;
 						}
 						k=4;
 					}
 					k++;
 				}
 			}
 		}
 	}

 	for(int row=0;row<4;row++){
 		for(int col=0;col<4;col++){
 			auto tiled=tables[row][col];
 			if(tiled->level==0){
 				int k=col+1;
 				while(k<4){
 					auto nextTiled=tables[row][k];
 					if(nextTiled->level!=0){
						tiled->setLevel(nextTiled->level);
						nextTiled->setLevel(0);
						tiled->setVisible(true);
						nextTiled->setVisible(false);
						hasMoved=true;
 						k=4;
 					}
 					k++;
 				}
 			}
 		}
 	}

	return hasMoved;
}

//向右
bool GameLayer::moveToRight(){
	bool hasMoved=false;
	//合成
	for(int row=0;row<4;row++){
		for(int col=3;col>=0;col--){
			auto tiled=tables[row][col];
			if(tiled->level!=0){
				int k=col-1;
				while(k>=0){
					auto nextTiled=tables[row][k];
					if(nextTiled->level!=0){
						if(tiled->level==nextTiled->level){
							tiled->setLevel(nextTiled->level+1);
							nextTiled->setLevel(0);
							nextTiled->setVisible(false);
							GameLayer::score+=Tiled::nums[tiled->level];
							this->lScore->setString(String::createWithFormat("%d",GameLayer::score)->getCString());
							hasMoved=true;
						}
						k=-1;
					}
					k--;
				}
			}
		}
	}

	for(int row=0;row<4;row++){
		for(int col=3;col>=0;col--){
			auto tiled=tables[row][col];
			if(tiled->level==0){
				int k=col-1;
				while(k>=0){
					auto nextTiled=tables[row][k];
					if(nextTiled->level!=0){
						tiled->setLevel(nextTiled->level);
						nextTiled->setLevel(0);
						tiled->setVisible(true);
						nextTiled->setVisible(false);
						hasMoved=true;
						k=-1;
					}
					k--;
				}
			}
		}
	}

	return hasMoved;
}

//向上
bool GameLayer::moveToTop(){
	bool hasMoved=false;
	//将数字相同的格子合一
	for(int col=0;col<4;col++){
		for(int row=3;row>=0;row--){
			auto tiled=tables[row][col];
			if(tiled->level!=0){
				int k=row-1;
				while(k>=0){
					auto nextTiled=tables[k][col];
					if(nextTiled->level!=0){
						if(tiled->level==nextTiled->level){
							tiled->setLevel(nextTiled->level+1);
							nextTiled->setLevel(0);
							nextTiled->setVisible(false);
							GameLayer::score+=Tiled::nums[tiled->level];
							this->lScore->setString(String::createWithFormat("%d",GameLayer::score)->getCString());
							hasMoved=true;
						}
						k=-1;
					}
					k--;
				}
			}
		}
	}


	//将有数的格子填入空格子
	for(int col=0;col<4;col++){
		for(int row=3;row>=0;row--){
			auto tiled=tables[row][col];
			if(tiled->level==0){
				int k=row-1;
				while(k>=0){
					auto nextTiled=tables[k][col];
					if(nextTiled->level!=0){
						tiled->setLevel(nextTiled->level);
						nextTiled->setLevel(0);
						tiled->setVisible(true);
						nextTiled->setVisible(false);
						hasMoved=true;
						k=-1;
					}
					k--;
				}
			}
		}
	}
	return hasMoved;
}


void GameLayer::addTiled(){
	auto cache=SpriteFrameCache::getInstance();
	//获取两个随机坐标
	default_random_engine e(time(NULL));
	uniform_int_distribution<unsigned> u(0,3);
	int row=0;
	int col=0;
	do{
		row=u(e);
		col=u(e);
	}while(tables[row][col]->level!=0);
	
	//添加砖块
	auto tiled=tables[row][col];
	int isFour=e()%10;
	if(isFour==0){
		tiled->level=2;
		tiled->backround->setDisplayFrame(cache->spriteFrameByName(String::createWithFormat("level2.png")->getCString()));
		tiled->label->setString("4");
		tiled->setVisible(true);
	}else{
		tiled->level=1;
		tiled->backround->setDisplayFrame(cache->spriteFrameByName(String::createWithFormat("level1.png")->getCString()));
		tiled->label->setString("2");
		tiled->setVisible(true);
	}

	tiled->setScale(0.5,0.5);
	tiled->runAction(ScaleTo::create(0.1f,1.0f));
}

bool GameLayer::isOver(){
	for(int row=0;row<4;row++){
		for(int col=0;col<4;col++){
			//判断是否存在空格子
			if(tables[row][col]->level==0){
				//有空格子肯定不会OVER
				return false;
			}
			//判断周围格子,如果存在相等的数字则不OVER
			//上
			int c=col;
			int r=row+1;
			if(r!=-1&&r!=4){
				if(tables[row][col]->level==tables[r][c]->level){
					return false;
				}
			}
			//左
			c=col-1;
			r=row;
			if(c!=-1&&c!=4){
				if(tables[row][col]->level==tables[r][c]->level){
					return false;
				}
			}
			//右
			c=col+1;
			r=row;
			if(c!=-1&&c!=4){
				if(tables[row][col]->level==tables[r][c]->level){
					return false;
				}
			}
			//下
			c=col;
			r=row-1;
			if(r!=-1&&r!=4){
				if(tables[row][col]->level==tables[r][c]->level){
					return false;
				}
			}
		}
	}
	return true;
}