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

		//��ӱ���
		auto background=Sprite::createWithSpriteFrame(cache->spriteFrameByName("background.png"));
		background->setPosition(Point(size.width/2,size.height/2));
		this->addChild(background);

		//��ӱ��ⱳ��
		auto headBg=Sprite::createWithSpriteFrame(cache->spriteFrameByName("title_bg.png"));
		headBg->setAnchorPoint(Point(0,1));
		headBg->setPosition(Point(0,640));
		this->addChild(headBg,1);

		//����˳������¿�ʼ��ť
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

		//���ש�鲿�ֱ���
		auto gameBg=Sprite::createWithSpriteFrame(cache->spriteFrameByName("game_bg.png"));
		gameBg->setAnchorPoint(Point::ZERO);
		gameBg->setPosition(Point(5,5));
		this->addChild(gameBg);

		//��ӷ�������
		auto scoreBg=Sprite::createWithSpriteFrame(cache->spriteFrameByName("score_bg.png"));
		scoreBg->setAnchorPoint(Point::ZERO);
		scoreBg->setPosition(Point(5,435));
		this->addChild(scoreBg);

		//��ӷ�����ʾ
		lScore=Label::create("0","Arial",40);
		lScore->setPosition(Point(size.width/4,470));
		this->addChild(lScore);

		//�����߷���ʾ
		int high=UserDefault::getInstance()->getIntegerForKey("HighScore",0);
		auto hScore=Label::create(String::createWithFormat("%d",high)->getCString(),"Arial",40);
		hScore->setPosition(Point(size.width/4*3,470));
		this->addChild(hScore);
		//��ʼ����Ϸ����
		gameInit();
		
		//��Ӽ�����
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
	//��ʼ��ש��
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

	//��ȡ�����������
	//c++11�������������ʽ
	default_random_engine e(time(NULL));
	//�������趨������������ķ�Χ��������0��3
	uniform_int_distribution<unsigned> u(0,3);
	int row1=u(e);
	int col1=u(e);
	int row2=u(e);
	int col2=u(e);
	//���ѭ���Ǳ�֤����ש������겻���ظ�
	do{
		row2=u(e);
		col2=u(e);
	}while(row1==row2&&col1==col2);

	//��ӵ�һ��ש��
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

	//��ӵڶ���ש��
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
		//�ж����»�������
		if(abs(touchUp.x-touchDown.x)>abs(touchUp.y-touchDown.y)){
			//���һ���
			if(touchUp.x-touchDown.x>0){
				//����
				log("toRight");
				hasMoved=moveToRight();
			}else{
				//����
				log("toLeft");
				hasMoved=moveToLeft();
			}
		}else{
			//���»���
			if(touchUp.y-touchDown.y>0){
				//����
				log("toTop");
				hasMoved=moveToTop();
			}else{
				//����
				log("toDown");
				hasMoved=moveToDown();
			}
		}

		if(hasMoved){
			addTiled();
		}

		if(isOver()){
			//��ŷ���
			int high=UserDefault::getInstance()->getIntegerForKey("HighScore",0);
			if(GameLayer::score>high){
				UserDefault::getInstance()->setIntegerForKey("HighScore",GameLayer::score);
				UserDefault::getInstance()->flush();
			}
			GameLayer::score=0;
			//�л�����
			Director::getInstance()->replaceScene(TransitionSlideInB::create(1.0f,OverScene::createScene()));
		}

	}
}

//�ĸ�����
bool GameLayer::moveToDown(){
	bool hasMoved=false;
	//��������ͬ�ĸ��Ӻ�һ
 	for(int col=0;col<4;col++){
 		for(int row=0;row<4;row++){
			//������ÿһ�λ�õķ���
 			auto tiled=tables[row][col];
			//�ҵ���Ϊ�յķ���
 			if(tiled->level!=0){
 				int k=row+1;
				//����һ����û�еȼ����������ȼ���ͬ��
 				while(k<4){
 					auto nextTiled=tables[k][col];
 					if(nextTiled->level!=0){
 						if(tiled->level==nextTiled->level){
							//�ҵ��ȼ������ש��ȼ���ͬ�ľͰ����Ǻϲ�
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


	//�������ĸ�������ո���
	for(int col=0;col<4;col++){
		for(int row=0;row<4;row++){
			//����ÿһ�ε�ש��
			auto tiled=tables[row][col];
			//�ҵ��ո���
			if(tiled->level==0){
				int k=row+1;
				while(k<4){
					auto nextTiled=tables[k][col];
					if(nextTiled->level!=0){
						//����Ϊ�յĸ����Ƶ�����
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

//����
bool GameLayer::moveToLeft(){ 
	bool hasMoved=false;
	//�ϳ�
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

//����
bool GameLayer::moveToRight(){
	bool hasMoved=false;
	//�ϳ�
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

//����
bool GameLayer::moveToTop(){
	bool hasMoved=false;
	//��������ͬ�ĸ��Ӻ�һ
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


	//�������ĸ�������ո���
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
	//��ȡ�����������
	default_random_engine e(time(NULL));
	uniform_int_distribution<unsigned> u(0,3);
	int row=0;
	int col=0;
	do{
		row=u(e);
		col=u(e);
	}while(tables[row][col]->level!=0);
	
	//���ש��
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
			//�ж��Ƿ���ڿո���
			if(tables[row][col]->level==0){
				//�пո��ӿ϶�����OVER
				return false;
			}
			//�ж���Χ����,���������ȵ�������OVER
			//��
			int c=col;
			int r=row+1;
			if(r!=-1&&r!=4){
				if(tables[row][col]->level==tables[r][c]->level){
					return false;
				}
			}
			//��
			c=col-1;
			r=row;
			if(c!=-1&&c!=4){
				if(tables[row][col]->level==tables[r][c]->level){
					return false;
				}
			}
			//��
			c=col+1;
			r=row;
			if(c!=-1&&c!=4){
				if(tables[row][col]->level==tables[r][c]->level){
					return false;
				}
			}
			//��
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