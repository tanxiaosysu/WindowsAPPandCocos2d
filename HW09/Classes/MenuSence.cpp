#include "MenuSence.h"
#include "GameSence.h"
USING_NS_CC;

Scene* MenuSence::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = MenuSence::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MenuSence::init() {

    if (!Layer::init()) {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto bg_sky = Sprite::create("menu-background-sky.jpg");
    bg_sky->setPosition(Vec2(origin.x + visibleSize.width / 2,
                             origin.y + visibleSize.height / 2 + 150));
    this->addChild(bg_sky, 0);

    auto bg = Sprite::create("menu-background.png");
    bg->setPosition(Vec2(origin.x + visibleSize.width/ 2,
                         origin.y + visibleSize.height / 2 - 60));
    this->addChild(bg, 0);

    auto miner = Sprite::create("menu-miner.png");
    miner->setPosition(Vec2(origin.x + 150,
                            origin.y + visibleSize.height / 2 - 60));
    this->addChild(miner, 1);

    auto leg = Sprite::createWithSpriteFrameName("miner-leg-0.png");
    Animate* legAnimate = Animate::create(
                              AnimationCache::getInstance()->getAnimation("legAnimation"));
    leg->runAction(RepeatForever::create(legAnimate));
    leg->setPosition(110 + origin.x, origin.y + 102);
    this->addChild(leg, 1);

    /* 标题 */
    auto title = Sprite::create("gold-miner-text.png");
    title->setPosition(Vec2(origin.x + visibleSize.width / 2,
                            origin.y + visibleSize.height - title->getContentSize().height));
    this->addChild(title, 1);

    /* 石头 */
    auto stone = Sprite::create("menu-start-gold.png");
    stone->setPosition(Vec2(origin.x + visibleSize.width - 180,
                            origin.y + visibleSize.height / 2 - 60));
    this->addChild(stone, 1);

    /* 开始按钮 */
    auto startItem = MenuItemImage::create(
                         "start-0.png",
                         "start-1.png",
                         CC_CALLBACK_1(MenuSence::menuStartCallBack, this));
    startItem->setPosition(Vec2::ZERO);

    /* 创建菜单 */
    auto menu = Menu::create(startItem, NULL);
    menu->setPosition(stone->getPosition() + Vec2(0, 40));
    this->addChild(menu, 2);

    return true;
}

/* 场景切换 */
void MenuSence::menuStartCallBack(Ref * pSender) {
    /* 创建游戏场景 */
    auto scene = GameSence::createScene();
    /* 切换(带渐隐效果) */
    Director::getInstance()->replaceScene(
        TransitionFade::create(0.5, scene));
}
