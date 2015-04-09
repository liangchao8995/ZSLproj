#include "LoadingScene.h"
#include "GameScene.h"

bool LoadingScene::init(){
	if(Scene::init()){
		return true;
	}else{
		return false;
	}
}

void LoadingScene::onEnter(){
	Size size=Director::getInstance()->getVisibleSize();
	//添加背景
	auto splash=Sprite::create("splash.png");
	splash->setPosition(Point(size.width/2,size.height/2));
	this->addChild(splash);

	//图片资源的异步加载
	TextureCache::getInstance()->addImageAsync("sucai.png",CC_CALLBACK_1(LoadingScene::loadComplete,this));
}

//图片加载完成后的回调函数
void LoadingScene::loadComplete(Texture2D *texture){
	//texture是加载完的图片资源
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sucai.plist",texture);
	log("load complete");

	//界面切换
	auto gameLayer=GameScene::create();
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f,gameLayer));
}