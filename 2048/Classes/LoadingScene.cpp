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
	//��ӱ���
	auto splash=Sprite::create("splash.png");
	splash->setPosition(Point(size.width/2,size.height/2));
	this->addChild(splash);

	//ͼƬ��Դ���첽����
	TextureCache::getInstance()->addImageAsync("sucai.png",CC_CALLBACK_1(LoadingScene::loadComplete,this));
}

//ͼƬ������ɺ�Ļص�����
void LoadingScene::loadComplete(Texture2D *texture){
	//texture�Ǽ������ͼƬ��Դ
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sucai.plist",texture);
	log("load complete");

	//�����л�
	auto gameLayer=GameScene::create();
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f,gameLayer));
}