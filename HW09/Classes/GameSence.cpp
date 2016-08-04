#include "GameSence.h"

USING_NS_CC;

Scene* GameSence::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = GameSence::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameSence::init() {

    if (!Layer::init()) {
        return false;
    }

    //add touch listener
    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(GameSence::onTouchBegan, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);


    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /* 背景图 */
    auto background = Sprite::create("level-background-0.jpg");
    background->setPosition(Vec2(origin.x + visibleSize.width / 2,
                                 origin.y + visibleSize.height / 2));
    this->addChild(background, 0);

    /* 石头Layer-第4层(最高) */
    this->stone = Sprite::create("stone.png");
    this->stone->setPosition(Vec2(origin.x + 560, origin.y + 480));
    this->stoneLayer = Layer::create();
    this->stoneLayer->setPosition(Vec2::ZERO);
    this->stoneLayer->addChild(this->stone);
    this->addChild(this->stoneLayer, 4);

    /* 标签-第4层(最高) */
    auto label = Label::createWithTTF("Shoot", "fonts/Marker Felt.ttf", 32);
    label->setPosition(Vec2::ZERO);

    auto shootItem = MenuItemLabel::create(label,
                                           CC_CALLBACK_1(GameSence::shootCallBack, this));
    shootItem->setPosition(Vec2::ZERO);

    auto menu = Menu::create(shootItem, NULL);
    menu->setPosition(
        Vec2(origin.x + visibleSize.width - 2 * label->getContentSize().width,
             origin.y + 480));
    this->addChild(menu, 4);

    /* 老鼠Layer-第2层 */
    this->mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
    this->mouse->setPosition(Vec2(visibleSize.width / 2, 0));
    this->mouseLayer = Layer::create();
    this->mouseLayer->setPosition(Vec2(0, visibleSize.height / 2));
    this->mouseLayer->addChild(this->mouse);
    this->addChild(this->mouseLayer, 2);

    /* 随机数种子 */
    srand(time(NULL));

    return true;
}

bool GameSence::onTouchBegan(Touch *touch, Event *unused_event) {
    auto location = touch->getLocation();

    /* 奶酪-第1层 */
    auto cheese = Sprite::create("cheese.png");
    cheese->setPosition(convertToNodeSpace(location));
    this->addChild(cheese, 1);
    /* 奶酪-渐隐动画 */
    auto fadeOut = FadeOut::create(3.0f);
    cheese->runAction(fadeOut);

    /* 老鼠-移动 */
    auto moveTo = MoveTo::create(0.5f, mouseLayer->convertToNodeSpace(location));
    this->mouse->runAction(moveTo);

    return true;
}

void GameSence::shootCallBack(Ref * pSender) {
    /* 世界坐标 */
    auto location = mouseLayer->convertToWorldSpace(this->mouse->getPosition());

    /* 生成石头 */
    auto newStone = Sprite::create("stone.png");
    newStone->setPosition(Vec2(this->stone->getPosition()));
    this->stoneLayer->addChild(newStone);

    /* 石头移动&渐隐 */
    auto moveTo = MoveTo::create(0.2f, stoneLayer->convertToNodeSpace(location));
    newStone->runAction(moveTo);
    auto fadeOut = FadeOut::create(0.8f);
    newStone->runAction(fadeOut);

    /* 生成钻石-第3层 */
    auto diamond = Sprite::create("diamond.png");
    diamond->setPosition(convertToNodeSpace(location));
    this->addChild(diamond, 3);

    /* 老鼠逃逸 */
    const Vec2 dir[4] = { Vec2(0, 50), Vec2(0, -50), Vec2(50, 0), Vec2(-50, 0) };
    auto moveBy = MoveBy::create(0.2f, dir[getRandomNumber(0, 3)]);
    this->mouse->runAction(moveBy);
}

/* 生成随机数 */
int GameSence::getRandomNumber(int min, int max) {
    return (min + rand() % (max - min + 1));
}
