#include "BreakoutScene.h"
#include "SimpleAudioEngine.h"
#include <string>
#include <sstream>

using namespace CocosDenshion;

#define TYPE_PLAYER 0x01
#define TYPE_ENEMY 0x02

#define COUNT_DOWN_TIME 30

#define TAG_EDGE 1
#define TAG_PLAYER 1
#define TAG_ENEMY1 10
#define TAG_ENEMY2 20
#define TAG_ENEMY3 30

void Breakout::setPhysicsWorld(PhysicsWorld* world) {
    m_world = world;
}

Scene* Breakout::createScene() {
    auto scene = Scene::createWithPhysics();
    /* 红色边框 */
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

    scene->getPhysicsWorld()->setGravity(Point(0, 0));

    auto layer = Breakout::create(scene->getPhysicsWorld());

    scene->addChild(layer);
    return scene;
}

bool Breakout::init(PhysicsWorld* world) {
    if (!Layer::init()) {
        return false;
    }

    this->setPhysicsWorld(world);
    visibleSize = Director::getInstance()->getVisibleSize();

    preloadMusic();
    playBgm();

    addBackground();
    addEdge();
    addPlayer();
    addUI();

    addContactListener();
    addTouchListener();
    addKeyboardListener();

    this->schedule(schedule_selector(Breakout::update), 1.0f);

    return true;
}

Breakout * Breakout::create(PhysicsWorld * world) {
    Breakout* pRet = new(std::nothrow) Breakout();
    if (pRet && pRet->init(world)) {
        pRet->autorelease();
        return pRet;
    }
    delete pRet;
    pRet = NULL;
    return NULL;
}

void Breakout::preloadMusic() {
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/bgm.mp3");
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/meet_stone.wav");
}

void Breakout::playBgm() {
    SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
    // 
}

void Breakout::addBackground() {
    auto bgsprite = Sprite::create("black_hole_bg0.jpg");
    bgsprite->setPosition(visibleSize / 2);
    bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width, visibleSize.height / bgsprite->getContentSize().height);
    this->addChild(bgsprite, 0);

    auto ps = ParticleSystemQuad::create("black_hole.plist");
    ps->setPosition(visibleSize / 2);
    this->addChild(ps);
}

void Breakout::addEdge() {
    auto edgeSp = Sprite::create();
    auto boundBody = PhysicsBody::createEdgeBox(visibleSize);
    boundBody->setDynamic(false);
    boundBody->setTag(TAG_EDGE);
    edgeSp->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
    edgeSp->setPhysicsBody(boundBody);
    this->addChild(edgeSp);
}

void Breakout::addPlayer() {
    player = Sprite::create("player.jpg");
    player->setAnchorPoint(Vec2(0.5, 0.5));
    player->setPhysicsBody(PhysicsBody::createCircle(35));
    // TODO set bitmask
    player->getPhysicsBody()->setCategoryBitmask(TYPE_PLAYER);
    player->getPhysicsBody()->setCollisionBitmask(TYPE_ENEMY);
    player->getPhysicsBody()->setContactTestBitmask(TYPE_ENEMY);

    player->setName("Player");

    player->setPosition(visibleSize / 2);
    player->getPhysicsBody()->setTag(TAG_PLAYER);
    player->getPhysicsBody()->setAngularVelocityLimit(0);
    addChild(player);
}

void Breakout::addUI() {
    // hp条
    Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
    Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

    // 使用hp条设置progressBar
    pT = ProgressTimer::create(sp);
    pT->setScaleX(90);
    pT->setAnchorPoint(Vec2(0, 0));
    pT->setType(ProgressTimerType::BAR);
    pT->setBarChangeRate(Point(1, 0));
    pT->setMidpoint(Point(0, 1));
    pT->setPercentage(100);
    pT->setPosition(Vec2(14 * pT->getContentSize().width, visibleSize.height - 2 * pT->getContentSize().height));
    addChild(pT, 3);
    sp0->setAnchorPoint(Vec2(0, 0));
    sp0->setPosition(Vec2(pT->getContentSize().width, visibleSize.height - sp0->getContentSize().height));
    addChild(sp0, 2);

    // 字体部分
    TTFConfig ttfConfig;
    ttfConfig.fontFilePath = "fonts/arial.ttf";
    ttfConfig.fontSize = 36;

    // 倒计时
    timeLabel = Label::createWithTTF(ttfConfig, "30");
    timeLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - timeLabel->getContentSize().height));
    // 倒计时的数字
    time = COUNT_DOWN_TIME;
    addChild(timeLabel, 3);
}

void Breakout::addContactListener() {
    auto touchListener = EventListenerPhysicsContact::create();
    touchListener->onContactBegin = CC_CALLBACK_1(Breakout::onConcactBegan, this);
    _eventDispatcher->addEventListenerWithFixedPriority(touchListener, 1);
}

void Breakout::addTouchListener() {
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(Breakout::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(Breakout::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(Breakout::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void Breakout::addKeyboardListener() {
    auto keboardListener = EventListenerKeyboard::create();
    keboardListener->onKeyPressed = CC_CALLBACK_2(Breakout::onKeyPressed, this);
    keboardListener->onKeyReleased = CC_CALLBACK_2(Breakout::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keboardListener, this);
}

bool Breakout::onTouchBegan(Touch *touch, Event *unused_event) {
    Vec2 postion = touch->getLocation();
    // do something
    //
    return true;
}

void Breakout::onTouchMoved(Touch *touch, Event *unused_event) {
    // TODO
    //
}

void Breakout::onTouchEnded(Touch *touch, Event *unused_event) {
    // TODO
    //
}

bool Breakout::onConcactBegan(PhysicsContact& contact) {
    // TODO
    auto body1 = contact.getShapeA()->getBody();
    auto body2 = contact.getShapeB()->getBody();
    auto sprite1 = (Sprite *)body1->getNode();
    auto sprite2 = (Sprite *)body2->getNode();

    /* 移除陨石 */
    if (sprite1 && sprite2) {
        /* 爆炸粒子 */
        ParticleExplosion* explosion = ParticleExplosion::create();
        if (sprite1->getPhysicsBody()->getTag() != TAG_PLAYER
            && sprite1->getPhysicsBody()->getTag() != TAG_EDGE) {
            explosion->setPosition(sprite1->getPosition());
            sprite1->removeFromParentAndCleanup(true);
        } else if (sprite2->getPhysicsBody()->getTag() != TAG_PLAYER
            && sprite2->getPhysicsBody()->getTag() != TAG_EDGE) {
            explosion->setPosition(sprite2->getPosition());
            sprite2->removeFromParentAndCleanup(true);
        }
        /* 添加粒子 */
        addChild(explosion);
    }

    /* 掉血 */
    const float health = 15.0f; /* 血量减少百分比 */
    auto percentage = pT->getPercentage() - health;
    if (percentage < 0) {
        /* 动画 */
        pT->runAction(ProgressTo::create(0.2f, 0));
        end(false);
    } else {
        pT->runAction(ProgressTo::create(0.2f, percentage));
    }

    /* 音效 */
    auto audio = SimpleAudioEngine::getInstance();
    audio->playEffect("music/meet_stone.wav", false, 1.0f, 1.0f, 1.0f);

    return true;
}

void Breakout::update(float f) {
    newEnemys();

    if (time > 0) {
        time--;
        std::stringstream s;
        s << time;
        timeLabel->setString(s.str());
    } else {
        end(true);
    }
}

void Breakout::end(bool ifWin) {
    /* 游戏终结, 停止调度 */
    this->unscheduleAllSelectors();
    _eventDispatcher->removeAllEventListeners();
    /* 对玩家实施人道毁灭 */
    this->removeChildByName("Player", true);
    /* 停止bgm */
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();

    TTFConfig ttfConfig;
    ttfConfig.fontFilePath = "fonts/arial.ttf";
    ttfConfig.fontSize = 36;
    std::string out = ifWin ? "You Win!" : "You Lose!";

    auto endLabel = Label::createWithTTF(ttfConfig, out);
    endLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

    addChild(endLabel, 3);
}

void Breakout::newEnemys() {
    if (enemys.size() > 50) return;
    int newNum = 2;
    while (newNum--) {
        int type = 0;
        if (CCRANDOM_0_1() > 0.85) {
            type = 2;
        } else if (CCRANDOM_0_1() > 0.6) {
            type = 1;
        }

        Point location = Vec2(0, 0);
        switch (rand() % 4) {
        case 0:
            location.y = visibleSize.height;
            location.x = rand() % (int)(visibleSize.width);
            break;
        case 1:
            location.x = visibleSize.width;
            location.y = rand() % (int)(visibleSize.height);
            break;
        case 2:
            location.y = 0;
            location.x = rand() % (int)(visibleSize.width);
            break;
        case 3:
            location.x = 0;
            location.y = rand() % (int)(visibleSize.height);
            break;
        default:
            break;
        }
        addEnemy(type, location);
    }
}

void Breakout::addEnemy(int type, Point p) {
    char path[100];
    int tag;
    switch (type) {
    case 0:
        sprintf(path, "stone1.png");
        tag = 10;
        break;
    case 1:
        sprintf(path, "stone2.png");
        tag = 20;
        break;
    case 2:
        sprintf(path, "stone3.png");
        tag = 30;
        break;
    default:
        sprintf(path, "stone1.png");
        tag = 10;
        break;
    }
    auto re = Sprite::create(path);
    re->setPhysicsBody(PhysicsBody::createCircle(re->getContentSize().height / 2));
    re->setAnchorPoint(Vec2(0.5, 0.5));
    re->setScale(0.5, 0.5);
    re->setPosition(p);
    // TODO set bitmask
    re->getPhysicsBody()->setCategoryBitmask(TYPE_ENEMY);
    re->getPhysicsBody()->setCollisionBitmask(TYPE_PLAYER);
    re->getPhysicsBody()->setContactTestBitmask(TYPE_PLAYER);

    re->setName("Enemy");

    re->getPhysicsBody()->setTag(tag);
    if (rand() % 100 < 50) {
        re->getPhysicsBody()->setVelocity((player->getPosition() - p) * (0.25));
    } else {
        re->getPhysicsBody()->setVelocity((Point(rand() % (int)(visibleSize.width - 100) + 50, rand() % (int)(visibleSize.height - 100) + 50) - p) * (0.25));
    }
    re->getPhysicsBody()->setAngularVelocity(CCRANDOM_0_1() * 10);
    enemys.pushBack(re->getPhysicsBody());
    addChild(re);
}

void Breakout::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
    switch (code) {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
        player->getPhysicsBody()->setVelocity(Point(-200, player->getPhysicsBody()->getVelocity().y));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
        player->getPhysicsBody()->setVelocity(Point(200, player->getPhysicsBody()->getVelocity().y));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_W:
        player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, 200));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_S:
        player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, -200));
        break;
    default:
        break;
    }
}

void Breakout::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
    switch (code) {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(-200, 0));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(200, 0));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_W:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, 200));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_S:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, -200));
        break;
    default:
        break;
    }
}
