#include "Tiled.h"

const int Tiled::nums[16]={0,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768};
Tiled::Tiled(){
	level=0;
}

bool Tiled::init(){
	bool bRet=false;
	do{
		CC_BREAK_IF(!Node::init());

		auto cache=SpriteFrameCache::getInstance();

		this->backround=Sprite::createWithSpriteFrame(cache->spriteFrameByName("level0.png"));
		this->backround->setPosition(Point::ZERO);
		this->addChild(backround);

		this->label=Label::create(String::createWithFormat("%d",Tiled::nums[level])->getCString(),"Arial",40);
		this->label->setPosition(Point::ZERO);
		this->addChild(label,1);

		bRet=true;
	}while(0);
	return bRet;
}

void Tiled::setLevel(int l){
	auto cache=SpriteFrameCache::getInstance();
	this->level=l;
	this->backround->setDisplayFrame(cache->spriteFrameByName(String::createWithFormat("level%d.png",level)->getCString()));
	this->label->setString(String::createWithFormat("%d",Tiled::nums[level])->getCString());
}