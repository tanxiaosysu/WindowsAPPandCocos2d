#include "HelloWorldScene.h"
#include "Monster.h"

#include <sstream>

#pragma execution_character_set("utf-8")
USING_NS_CC;

#define MIN_FLOAT 0.00001f
#define MAX_MONSTER_COUNT 15

Scene* HelloWorld::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init() {
    //////////////////////////////
    // 1. super init first
    if (!Layer::init()) {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

    /* 地图 */
    TMXTiledMap * tmx = TMXTiledMap::create("map.tmx");
    tmx->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
    tmx->setAnchorPoint(Vec2(0.5f, 0.5f));
    tmx->setScale(Director::getInstance()->getContentScaleFactor());
    this->addChild(tmx, 0);

    //创建一张贴图
    auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
    //从贴图中以像素单位切割，创建关键帧
    auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
    //使用第一帧创建精灵
    player = Sprite::createWithSpriteFrame(frame0);
    player->setPosition(Vec2(origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height / 2));
    addChild(player, 3);


    TTFConfig ttfConfig;
    ttfConfig.fontFilePath = "fonts/arial.ttf";
    ttfConfig.fontSize = 36;

    //杀怪数
    killCount = Label::createWithTTF(ttfConfig, "0");
    killCount->setPosition(Vec2(origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height - killCount->getContentSize().height));
    addChild(killCount);

    //周期性更新怪物调度器
    schedule(schedule_selector(HelloWorld::updateMonster), 3.0f, kRepeatForever, 0);

    //hp条
    Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
    Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

    //使用hp条设置progressBar
    pT = ProgressTimer::create(sp);
    pT->setScaleX(90);
    pT->setAnchorPoint(Vec2(0, 0));
    pT->setType(ProgressTimerType::BAR);
    pT->setBarChangeRate(Point(1, 0));
    pT->setMidpoint(Point(0, 1));
    pT->setPercentage(100);
    pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
    addChild(pT, 1);
    sp0->setAnchorPoint(Vec2(0, 0));
    sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
    addChild(sp0, 0);

    //静态动画
    idle.reserve(1);
    idle.pushBack(frame0);

    //攻击动画
    attack.reserve(17);
    for (int i = 0; i < 17; i++) {
        auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
        attack.pushBack(frame);
    }

    //死亡动画
    auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
    dead.reserve(22);
    for (int i = 0; i < 22; i++) {
        auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
        dead.pushBack(frame);
    }

    //运动动画
    auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
    for (int i = 0; i < 8; i++) {
        auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
        run.pushBack(frame);
    }

    //Label
    auto menuLabel1 = Label::createWithTTF(ttfConfig, "W");
    auto menuLabel2 = Label::createWithTTF(ttfConfig, "S");
    auto menuLabel3 = Label::createWithTTF(ttfConfig, "A");
    auto menuLabel4 = Label::createWithTTF(ttfConfig, "D");
    auto menuLabel5 = Label::createWithTTF(ttfConfig, "X");
    //menuItem
    auto item1 = MenuItemLabel::create(menuLabel1, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'W'));
    auto item2 = MenuItemLabel::create(menuLabel2, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'S'));
    auto item3 = MenuItemLabel::create(menuLabel3, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'A'));
    auto item4 = MenuItemLabel::create(menuLabel4, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'D'));
    auto item5 = MenuItemLabel::create(menuLabel5, CC_CALLBACK_1(HelloWorld::actionEvent, this, 'X'));

    item3->setPosition(Vec2(origin.x + item3->getContentSize().width, origin.y + item3->getContentSize().height));
    item4->setPosition(Vec2(item3->getPosition().x + 3 * item4->getContentSize().width, item3->getPosition().y));
    item2->setPosition(Vec2(item3->getPosition().x + 1.5*item2->getContentSize().width, item3->getPosition().y));
    item1->setPosition(Vec2(item2->getPosition().x, item2->getPosition().y + item1->getContentSize().height));
    item5->setPosition(Vec2(origin.x + visibleSize.width - item5->getContentSize().width, item1->getPosition().y));

    menu = Menu::create(item1, item2, item3, item4, item5, NULL);
    menu->setPosition(Vec2(0, 0));
    addChild(menu, 1);

    lastCid = 'n';
    monsterCount = 0;
    //杀怪数的数字
    intKillCount = 0;
    return true;
}


void HelloWorld::moveEvent(Ref*, char cid) {
    /* 碰撞检测 */
    scheduleOnce(schedule_selector(HelloWorld::hitByMonster), 0);

    const float distance = 30; /* 移动距离 */
    const Vec2 moveList[4] = { /* 移动向量, 顺序依次为WASD */
        Vec2(0, distance), Vec2(-1 * distance, 0), Vec2(0, -1 * distance), Vec2(distance, 0)
    };

    /* 动作 */
    auto animation = Animation::createWithSpriteFrames(run, 0.1f);
    auto animate = Animate::create(animation);

    auto playerPos = player->getPosition(); /* 角色坐标 */
    switch (cid) {
    case 'W':
        /* 条件判断是否处于边界, 下同 */
        if (visibleSize.height - playerPos.y - distance < MIN_FLOAT) {
            /* 处于边界则只播放动画不移动, 下同 */
            player->runAction(animate);
        } else {
            /* 不处于边界则按moveList的指定方位移动, 下同 */
            player->runAction(Spawn::create(MoveBy::create(0.8f, moveList[0]), animate, NULL));
        }
        break;
    case 'A':
        if (lastCid != 'A') {
            player->setFlipX(true);
        }
        if (playerPos.x - distance < MIN_FLOAT) {
            player->runAction(animate);
        } else {
            player->runAction(Spawn::create(MoveBy::create(0.8f, moveList[1]), animate, NULL));
        }
        break;
    case 'S':
        if (playerPos.y - distance < MIN_FLOAT) {
            player->runAction(animate);
        } else {
            player->runAction(Spawn::create(MoveBy::create(0.8f, moveList[2]), animate, NULL));
        }
        break;
    case 'D':
        if (lastCid != 'D') {
            player->setFlipX(false);
        }
        if (visibleSize.width - playerPos.x - distance < MIN_FLOAT) {
            player->runAction(animate);
        } else {
            player->runAction(Spawn::create(MoveBy::create(0.8f, moveList[3]), animate, NULL));
        }
        break;
    default:
        break;
    }
    lastCid = cid;
}


void HelloWorld::actionEvent(Ref*, char cid) {
    const float health = 15.0f; /* 血量增减百分比 */
    float percentage;

    /* 动作 */
    auto attackAnimation = Animation::createWithSpriteFrames(attack, 0.05f);
    auto deadAnimation = Animation::createWithSpriteFrames(dead, 0.05f);

    auto attackAnimate = Animate::create(attackAnimation);
    auto deadAnimate = Animate::create(deadAnimation);

    switch (cid) {
    case 'X':
        player->runAction(attackAnimate);
        /* 打怪 */
        if (hitMonster()) {
            /* 控制血量不超过100% */
            percentage = pT->getPercentage() + health;
            /* 动画 */
            pT->runAction(ProgressTo::create(0.2f, 100.0f - percentage < MIN_FLOAT ? 100.0f : percentage));
        }
        break;
    case 'Y':
        player->runAction(deadAnimate);
        /* 控制血量不低于0% */
        percentage = pT->getPercentage() - health;
        /* 动画 */
        pT->runAction(ProgressTo::create(0.2f, percentage < MIN_FLOAT ? 0 : percentage));
        break;
    default:
        break;
    }
}

/* 更新怪函数 */
void HelloWorld::updateMonster(float dt) {
    auto fac = Factory::getInstance();
    /* 怪物不超过15个, 否则不会产生新怪 */
    if (monsterCount < MAX_MONSTER_COUNT) {
        monsterCount++;

        auto m = fac->createMonster();
        float x = random(origin.x, visibleSize.width);
        float y = random(origin.y, visibleSize.height);
        m->setPosition(x, y);
        addChild(m, 3);
    }
    /* 怪物移动 */
    fac->moveMonster(player->getPosition(), 1.0f);
    /* 碰撞检测 */
    scheduleOnce(schedule_selector(HelloWorld::hitByMonster), 0);
}

/* 打怪函数, 打到则移除怪返回true, 打不到返回false */
bool HelloWorld::hitMonster() {
    auto fac = Factory::getInstance();

    Rect playerRect = player->getBoundingBox();
    Rect attackRect = Rect(playerRect.getMinX() - 40,
        playerRect.getMinY(),
        playerRect.getMaxX() - playerRect.getMinX() + 80,
        playerRect.getMaxY() - playerRect.getMinY());

    Sprite* collision = fac->collider(attackRect);
    if (collision != NULL) {
        fac->removeMonster(collision);
        /* 怪物数更新 */
        monsterCount--;
        /* 击杀数更新 */
        updateKillCount();
        return true;
    }
    return false;
}

/* 被怪打函数 */
void HelloWorld::hitByMonster(float dt) {
    auto fac = Factory::getInstance();
    Sprite* collision = fac->collider(player->getBoundingBox());
    if (collision != NULL) { /* 被怪打了 */
        fac->removeMonster(collision);
        actionEvent(this, 'Y');
    }
}

/* 杀怪数更新函数 */
void HelloWorld::updateKillCount() {
    /* 杀怪数+1, Label更新内容 */
    intKillCount++;
    std::stringstream s;
    s << intKillCount;
    killCount->setString(s.str());
}
