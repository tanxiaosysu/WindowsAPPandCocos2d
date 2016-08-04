#include "Global.h"
#include "GameScene.h"
#include "EndScene.h"

#include "SimpleAudioEngine.h"

#include <string>
#include <cstdlib>

using namespace CocosDenshion;

/* Sprite的层次 */
#define LAYER_BG 0
#define LAYER_GAME 1
#define LAYER_UI 2
#define LAYER_UI_TOP 3

#define WIDTH_BG (visibleSize.height - 1)

/* Sprite的分组 */
#define GROUP_PLAYER -1
#define GROUP_ENEMY 0x02

/* Sprite的类型 */
#define TYPE_EDGE 0x01
#define TYPE_PLAYER 0x02
#define TYPE_BULLET 0x04
#define TYPE_ENEMY 0x08

/* Sprite的标签 */
#define TAG_EDGE 0
#define TAG_PLAYER 1
#define TAG_BULLET 2
#define TAG_ENEMY1 10
#define TAG_ENEMY2 20
#define TAG_ENEMY3 30
#define TAG_BOSS 40

/* 倒计时 */
#define COUNT_DOWN_TIME 60

#define SPEED_BULLET 250

void GameScene::setPhysicsWorld(PhysicsWorld* world) {
    m_world = world;
}

Scene* GameScene::createScene() {
    auto scene = Scene::createWithPhysics();
    /* 红色边框 */
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

    scene->getPhysicsWorld()->setGravity(Point(0, 0));

    auto layer = GameScene::create(scene->getPhysicsWorld());

    scene->addChild(layer);
    return scene;
}

bool GameScene::init(PhysicsWorld* world) {
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

    enemys.clear();
    bosses.clear();
    for (uint16 i = 0; i < ENEMY_COUNT; i++) {
        enemysHealth[i] = 0;
    }
    for (uint16 i = 0; i < BOSS_PART_COUNT; i++) {
        bossHealth[i] = 0;
    }

    addContactListener();
    addTouchListener();
    addKeyboardListener();

    this->schedule(schedule_selector(GameScene::updatePerSecond), 1.0f);
    this->schedule(schedule_selector(GameScene::updateBackground), 0.01f);
    /* 定时刷怪和刷Boss */
    this->schedule(schedule_selector(GameScene::updateEnemy), 5.0f, 6, 0.0f);
    this->scheduleOnce(schedule_selector(GameScene::createBoss), 40.0f);
    return true;
}

GameScene * GameScene::create(PhysicsWorld * world) {
    GameScene* pRet = new(std::nothrow) GameScene();
    if (pRet && pRet->init(world)) {
        pRet->autorelease();
        return pRet;
    }
    //delete pRet;
    //pRet = NULL;
    //return NULL;
}

void GameScene::preloadMusic() {
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/bgm.mp3");
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/fire.wav");
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/meet_stone.wav");
}

void GameScene::playBgm() {
    SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
    //
}

void GameScene::addBackground() {
    auto size = Director::getInstance()->getWinSize();
    backGround0 = Sprite::create("bg.jpg");
    backGround1 = Sprite::create("bg.jpg");
    backGround0->setAnchorPoint(Vec2(0, 0));
    backGround1->setAnchorPoint(Vec2(0, 0));
    backGround0->setPosition(Vec2(0, 0));
    backGround1->setPosition(Vec2(0, WIDTH_BG));

    backGround0->setScaleX(size.width / backGround0->getTextureRect().getMaxX());
    backGround0->setScaleY(size.height / backGround0->getTextureRect().getMaxY());
    backGround1->setScaleX(size.width / backGround1->getTextureRect().getMaxX());
    backGround1->setScaleY(size.height / backGround1->getTextureRect().getMaxY());

    this->addChild(backGround0, LAYER_BG);
    this->addChild(backGround1, LAYER_BG);

    //auto ps = ParticleSystemQuad::create("black_hole.plist");
    //ps->setPosition(visibleSize / 2);
    //this->addChild(ps);
}

void GameScene::addEdge() {
    auto edgeSp = Sprite::create();
    const Point edge[] = {
        Point(0, visibleSize.height), Point::ZERO,
        Point(visibleSize.width, 0), visibleSize
    };
    //auto boundBody = PhysicsBody::createEdgePolygon(edge, 3);
    auto boundBody = PhysicsBody::createEdgeBox(visibleSize);
    boundBody->setDynamic(false);
    boundBody->setCategoryBitmask(TYPE_EDGE);
    boundBody->setContactTestBitmask(TYPE_BULLET | TYPE_ENEMY);
    boundBody->setTag(TAG_EDGE);
    edgeSp->setPosition(Point(visibleSize / 2));
    edgeSp->setPhysicsBody(boundBody);
    this->addChild(edgeSp);
}

void GameScene::addPlayer() {
    player = Sprite::create("player.png");
    player->setAnchorPoint(Vec2(0.5, 0.5));
    player->setPhysicsBody(PhysicsBody::createCircle(25));
    // TODO set bitmask
    player->getPhysicsBody()->setCategoryBitmask(TYPE_PLAYER);
    //player->getPhysicsBody()->setCollisionBitmask(TYPE_ENEMY);
    player->getPhysicsBody()->setGroup(GROUP_PLAYER);
    player->getPhysicsBody()->setContactTestBitmask(TYPE_ENEMY | TYPE_BULLET);

    player->getPhysicsBody()->setRotationEnable(false);

    player->setName("Player");

    player->setPosition(visibleSize / 2);
    player->getPhysicsBody()->setTag(TAG_PLAYER);
    player->getPhysicsBody()->setAngularVelocityLimit(0);
    addChild(player, LAYER_GAME);
}

void GameScene::addUI() {
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
    addChild(pT, LAYER_UI_TOP);
    sp0->setAnchorPoint(Vec2(0, 0));
    sp0->setPosition(Vec2(pT->getContentSize().width, visibleSize.height - sp0->getContentSize().height));
    addChild(sp0, LAYER_UI);

    // 字体部分
    TTFConfig ttfConfig;
    ttfConfig.fontFilePath = "fonts/arial.ttf";
    ttfConfig.fontSize = 36;

    // 倒计时的数字
    time = COUNT_DOWN_TIME;
    // 倒计时
    timeLabel = Label::createWithTTF(ttfConfig, "time:" + Global::integerToString(time));
    timeLabel->setPosition(Vec2(visibleSize.width - timeLabel->getContentSize().width,
        visibleSize.height - timeLabel->getContentSize().height - pT->getContentSize().height));
    addChild(timeLabel, LAYER_UI_TOP);

    // 分数
    scoreLabel = Label::createWithTTF(ttfConfig, "score:" + Global::integerToString(Global::score));
    scoreLabel->setPosition(Vec2(scoreLabel->getContentSize().width / 2 + 20,
        visibleSize.height - scoreLabel->getContentSize().height - pT->getContentSize().height));
    addChild(scoreLabel, LAYER_UI_TOP);
}

void GameScene::addKeyboardListener() {
    auto keboardListener = EventListenerKeyboard::create();
    keboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    keboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keboardListener, this);
}

void GameScene::addContactListener() {
    auto touchListener = EventListenerPhysicsContact::create();
    touchListener->onContactBegin = CC_CALLBACK_1(GameScene::onConcactBegan, this);
    _eventDispatcher->addEventListenerWithFixedPriority(touchListener, 1);
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
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
    case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
        fire();
        break;
    default:
        break;
    }
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
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

bool GameScene::onConcactBegan(PhysicsContact& contact) {
    // TODO
    auto body1 = contact.getShapeA()->getBody();
    auto body2 = contact.getShapeB()->getBody();
    auto sprite1 = (Sprite *)body1->getNode();
    auto sprite2 = (Sprite *)body2->getNode();

    /* 移除陨石 */
    if (sprite1 && sprite2) {
        /* 爆炸粒子 */
        ParticleExplosion* explosion = ParticleExplosion::create();
        explosion->setScale(0.5);

        auto sp1group = sprite1->getPhysicsBody()->getGroup();
        auto sp2group = sprite2->getPhysicsBody()->getGroup();
        auto sp1tag = sprite1->getPhysicsBody()->getTag();
        auto sp2tag = sprite2->getPhysicsBody()->getTag();

        if (sp2tag == TAG_EDGE) {
            if (sp1tag == TAG_BULLET || sprite1->getPhysicsBody()->getCategoryBitmask() == TYPE_ENEMY) {
                if (sprite1->getPhysicsBody()->getCategoryBitmask() == TYPE_ENEMY) {
                    enemysHealth[atoi(sprite1->getName().c_str())] = 0;
                }
                sprite1->removeFromParentAndCleanup(true);
            }
            return false;
        } else if (sp1tag == TAG_EDGE) {
            if (sp2tag == TAG_BULLET || sprite2->getPhysicsBody()->getCategoryBitmask() == TYPE_ENEMY) {
                if (sprite2->getPhysicsBody()->getCategoryBitmask() == TYPE_ENEMY) {
                    enemysHealth[atoi(sprite2->getName().c_str())] = 0;
                }
                sprite2->removeFromParentAndCleanup(true);
            }
            return false;
        }
        if (sp1group == GROUP_ENEMY) {
            if (sp2tag == TAG_PLAYER) {
                /* 玩家受到伤害 */
                hurt(sp1tag);
                if (sprite1->getPhysicsBody()->getCategoryBitmask() == TYPE_BULLET) {
                    /* 移除敌人的子弹 */
                    sprite1->removeFromParentAndCleanup(true);
                    return false;
                }
            } else if (sp2tag == TAG_BULLET) {
                sprite2->removeFromParentAndCleanup(true);
                if (sprite1->getPhysicsBody()->getCategoryBitmask() == TYPE_BULLET) {
                    /* 撞到敌人子弹-抵消 */
                    sprite1->removeFromParentAndCleanup(true);
                    return false;
                } else {
                    /* 打击到敌人-敌人死亡-加分 */
                    if (sp1tag == TAG_BOSS) {
                        if (bossHealth[atoi(sprite1->getName().c_str())] == 1) {
                            Global::score += sp1tag * 10;
                        }
                    } else {
                        if (enemysHealth[atoi(sprite1->getName().c_str())] == 1) {
                            Global::score += sp1tag;
                        }
                    }
                    scoreLabel->setString("score:" + Global::integerToString(Global::score));
                }
            }
            /* 玩家/玩家的子弹碰到怪会掉血 */
            if (sprite1->getPhysicsBody()->getCategoryBitmask() == TYPE_ENEMY) {
                if (sp1tag == TAG_BOSS) {
                    bossHealth[atoi(sprite1->getName().c_str())]--;
                    if (!bossHealth[atoi(sprite1->getName().c_str())]) {
                        explosion->setPosition(sprite1->getPosition()); /* 粒子效果 */
                        addChild(explosion); /* 添加粒子 */
                        sprite1->removeFromParentAndCleanup(true); /* 摧毁 */
                    }
                } else {
                    enemysHealth[atoi(sprite1->getName().c_str())]--;
                    if (!enemysHealth[atoi(sprite1->getName().c_str())]) {
                        explosion->setPosition(sprite1->getPosition()); /* 粒子效果 */
                        addChild(explosion); /* 添加粒子 */
                        sprite1->removeFromParentAndCleanup(true); /* 摧毁 */
                    }
                }
            }
        } else if (sp2group == GROUP_ENEMY) {
            if (sp1tag == TAG_PLAYER) {
                /* 玩家受到伤害 */
                hurt(sp2tag);
                if (sprite2->getPhysicsBody()->getCategoryBitmask() == TYPE_BULLET) {
                    /* 移除敌人的子弹 */
                    sprite2->removeFromParentAndCleanup(true);
                    return false;
                }
            } else if (sp1tag == TAG_BULLET) {
                sprite1->removeFromParentAndCleanup(true);
                if (sprite2->getPhysicsBody()->getCategoryBitmask() == TYPE_BULLET) {
                    /* 撞到敌人子弹-抵消 */
                    sprite2->removeFromParentAndCleanup(true);
                    return false;
                } else {
                    /* 打击到敌人-敌人死亡-加分 */
                    if (sp2tag == TAG_BOSS) {
                        if (bossHealth[atoi(sprite2->getName().c_str())] == 1) {
                            Global::score += sp2tag * 10;
                        }
                    } else {
                        if (enemysHealth[atoi(sprite2->getName().c_str())] == 1) {
                            Global::score += sp2tag;
                        }
                    }
                    scoreLabel->setString("score:" + Global::integerToString(Global::score));
                }
            }
            /* 玩家/玩家的子弹碰到怪会掉血 */
            if (sprite2->getPhysicsBody()->getCategoryBitmask() == TYPE_ENEMY) {
                if (sp2tag == TAG_BOSS) {
                    bossHealth[atoi(sprite2->getName().c_str())]--;
                    if (!bossHealth[atoi(sprite2->getName().c_str())]) {
                        explosion->setPosition(sprite2->getPosition()); /* 粒子效果 */
                        addChild(explosion); /* 添加粒子 */
                        sprite2->removeFromParentAndCleanup(true); /* 摧毁 */
                    }
                } else {
                    enemysHealth[atoi(sprite2->getName().c_str())]--;
                    if (!enemysHealth[atoi(sprite2->getName().c_str())]) {
                        explosion->setPosition(sprite2->getPosition()); /* 粒子效果 */
                        addChild(explosion); /* 添加粒子 */
                        sprite2->removeFromParentAndCleanup(true); /* 摧毁 */
                    }
                }
            }
        }
    }

    /* 音效 */
    auto audio = SimpleAudioEngine::getInstance();
    audio->playEffect("music/meet_stone.wav", false, 1.0f, 1.0f, 1.0f);

    return false;
}

void GameScene::updatePerSecond(float f) {
    updateEnemyPerWave();
    if (time > 0) {
        time--;
        timeLabel->setString("time:" + Global::integerToString(time));
        if (time < 20 && bossHealth[0] == 0) {
            end(true);
        }
    } else {
        /* 杀死BOSS本体才能获胜 */
        end((bossHealth[0] == 0));
    }
}

void GameScene::updateBackground(float f) {
    const float move = 1.0f;
    auto bg0Pos = backGround0->getPosition();
    auto bg1Pos = backGround1->getPosition();
    backGround0->setPosition(Vec2(bg0Pos.x, bg0Pos.y - move));
    backGround1->setPosition(Vec2(bg1Pos.x, bg1Pos.y - move));
    if (bg1Pos.y < 0) {
        backGround0->setPosition(Vec2(bg0Pos.x, bg1Pos.y + WIDTH_BG));
    }
    if (bg0Pos.y < 0) {
        backGround1->setPosition(Vec2(bg1Pos.x, bg0Pos.y + WIDTH_BG));
    }
}

void GameScene::updateEnemy(float f) {
    static int8 wave = 0;
    newEnemys(wave);
    wave = (wave + 1) % 7;
}

void GameScene::updateEnemyPerWave() {
    int8 num = 0;
    /* 遍历当前的敌人, 每个敌人都开枪 */
    for (auto i: enemys) {
        if (enemysHealth[num] > 0) {
            Sprite * enemyBullet = Sprite::create("bullets.png", Rect(425, 253, 15, 25));
            enemyBullet->setPosition(i->getPosition().x, i->getPosition().y);
            enemyBullet->setAnchorPoint(Vec2(0.5, 0.5));

            auto physicsBody = PhysicsBody::createBox(
                Size(enemyBullet->getContentSize().width / 2, enemyBullet->getContentSize().height / 2));
            physicsBody->setRotationEnable(false);
            physicsBody->setVelocity(Vec2(0, -SPEED_BULLET));
            physicsBody->setCategoryBitmask(TYPE_BULLET);
            physicsBody->setGroup(GROUP_ENEMY);
            physicsBody->setContactTestBitmask(TYPE_PLAYER | TYPE_BULLET | TYPE_EDGE);
            physicsBody->setTag(i->getCategoryBitmask());
            enemyBullet->setPhysicsBody(physicsBody);

            addChild(enemyBullet, LAYER_GAME);
        } else if (enemysHealth[num] < 0) {
            enemysHealth[num] = 0;
        }
        num++;
    }
}

void GameScene::createBoss(float f) {
    newEnemys(7);
}

void GameScene::updateBoss(float f) {
    int8 num = 0;
    /* 遍历当前的敌人, 每个敌人都开枪 */
    for (auto i : bosses) {
        if (bossHealth[num] > 0) {
            Sprite * bossBullet = Sprite::create("bullets.png", Rect(398, 210, 12, 22));
            bossBullet->setPosition(i->getPosition().x, i->getPosition().y - 10);
            bossBullet->setAnchorPoint(Vec2(0.5, 0.5));

            auto physicsBody = PhysicsBody::createBox(
                Size(bossBullet->getContentSize().width, bossBullet->getContentSize().height));
            physicsBody->setRotationEnable(false);
            physicsBody->setVelocity(Vec2(0, -SPEED_BULLET));
            physicsBody->setCategoryBitmask(TYPE_BULLET);
            physicsBody->setGroup(GROUP_ENEMY);
            physicsBody->setContactTestBitmask(TYPE_PLAYER | TYPE_BULLET | TYPE_EDGE);
            physicsBody->setTag(i->getCategoryBitmask());
            bossBullet->setPhysicsBody(physicsBody);
            addChild(bossBullet, LAYER_GAME);
            Vec2 pos = i->getPosition();
        } else if (bossHealth[num] < 0) {
            bossHealth[num] = 0;
        }
        num++;
    }
}

void GameScene::end(bool ifWin) {
    /* 游戏终结, 停止调度 */
    this->unscheduleAllSelectors();
    _eventDispatcher->removeAllEventListeners();
    /* 对玩家实施人道毁灭 */
    this->removeChildByName("Player", true);
    /* 停止bgm */
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();

    Global::ifWin = ifWin;
    /* 创建游戏场景 */
    auto scene = EndScene::createScene();
    /* 切换(带渐隐效果) */
    Director::getInstance()->replaceScene(TransitionFade::create(0.1f, scene));
}

void GameScene::newEnemys(int8 wave) {
    switch (wave) {
    case 7:
        addBoss();
        break;
    case 6: /* 8个3级敌人 */
        addEnemy(2, Vec2(30, 620));
        addEnemy(2, Vec2(450, 620));
        addEnemy(2, Vec2(90, 650));
        addEnemy(2, Vec2(390, 650));
        addEnemy(2, Vec2(150, 670));
        addEnemy(2, Vec2(330, 670));
        addEnemy(2, Vec2(210, 690));
        addEnemy(2, Vec2(270, 690));
        break;
    case 5: /* 4个2级敌人+2个1级敌人 */
        addEnemy(2, Vec2(90, 690));
        addEnemy(2, Vec2(390, 690));
    case 4: /* 2个2级敌人+2个1级敌人 */
        addEnemy(2, Vec2(150, 690));
        addEnemy(2, Vec2(330, 690));
        addEnemy(1, Vec2(210, 690));
        addEnemy(1, Vec2(270, 690));
        break;
    case 3: /* 2个3级敌人+4个2级敌人+2个1级敌人 */
        addEnemy(2, Vec2(30, visibleSize.height - 40));
        addEnemy(2, Vec2(450, visibleSize.height - 40));
    case 2: /* 4个2级敌人+2个1级敌人 */
        addEnemy(1, Vec2(90, visibleSize.height - 50));
        addEnemy(1, Vec2(390, visibleSize.height - 50));
    case 1: /* 2个2级敌人+2个1级敌人 */
        addEnemy(1, Vec2(150, visibleSize.height - 80));
        addEnemy(1, Vec2(330, visibleSize.height - 80));
    case 0: /* 2个1级敌人 */
    default:
        addEnemy(0, Vec2(210, visibleSize.height - 80));
        addEnemy(0, Vec2(270, visibleSize.height - 80));
        break;
    }
    //if (enemys.size() > 50) return;

}

void GameScene::addEnemy(int type, Point p) {
    const int health[3] = { 2, 3, 5 };
    std::string path;
    int tag;
    switch (type) {
    case 2:
        path = "enemy3.png";
        tag = TAG_ENEMY3;
        break;
    case 1:
        path = "enemy2.png";
        tag = TAG_ENEMY2;
        break;
    case 0:
    default:
        path = "enemy1.png";
        tag = TAG_ENEMY1;
        break;
    }
    auto re = Sprite::create(path);
    re->setPhysicsBody(PhysicsBody::createCircle(re->getContentSize().height / 2));
    re->setAnchorPoint(Vec2(0.5, 0.5));
    re->setScale(0.5, 0.5);
    re->setPosition(p);
    // TODO set bitmask
    re->getPhysicsBody()->setCategoryBitmask(TYPE_ENEMY);
    //re->getPhysicsBody()->setCollisionBitmask(TYPE_PLAYER);
    re->getPhysicsBody()->setGroup(GROUP_ENEMY);
    re->getPhysicsBody()->setContactTestBitmask(TYPE_PLAYER | TYPE_BULLET | TYPE_EDGE);

    re->setName(Global::integerToString(enemys.size()));

    re->getPhysicsBody()->setTag(tag);
    re->getPhysicsBody()->setVelocity(Vec2(0, -100));
    re->getPhysicsBody()->setAngularVelocityLimit(0);
    enemys.pushBack(re->getPhysicsBody());
    enemysHealth[enemys.size() - 1] = health[type];
    addChild(re, LAYER_GAME);
}

void GameScene::addBoss() {
    /* boss本体 */
    auto boss = Sprite::create("boss.png");
    boss->setAnchorPoint(Vec2(0.5, 0.5));
    boss->setPosition(visibleSize.width / 2, visibleSize.height - boss->getContentSize().height / 2);
    boss->setName("0");
    auto physicsBody = PhysicsBody::createBox(
        Size(boss->getContentSize().width / 2, boss->getContentSize().height / 2));
    physicsBody->setRotationEnable(false);
    physicsBody->setVelocity(Vec2::ZERO);
    physicsBody->setCategoryBitmask(TYPE_ENEMY);
    physicsBody->setGroup(GROUP_ENEMY);
    physicsBody->setContactTestBitmask(TYPE_BULLET | TYPE_PLAYER);
    physicsBody->setTag(TAG_BOSS);
    boss->setPhysicsBody(physicsBody);
    bosses.pushBack(boss->getPhysicsBody());
    addChild(boss, LAYER_GAME);
    bossHealth[0] = 10;
    /* boss组件 */
    for (int8 i = 1; i < 5; i++) {
        auto bossPart = Sprite::create("bosspart.png", Rect(0, 0, 48, 58));
        bossPart->setAnchorPoint(Vec2(0.5, 0.5));
        bossPart->setPosition(boss->getPosition() - Vec2(300 - 120 * i, bossPart->getContentSize().height));
        bossPart->setName(Global::integerToString(i));
        bossPart->runAction(RotateTo::create(0.0, 180.0));
        auto physicsBody = PhysicsBody::createBox(
            Size(bossPart->getContentSize().width / 2, bossPart->getContentSize().height / 2));
        physicsBody->setRotationEnable(false);
        physicsBody->setVelocity(Vec2::ZERO);
        physicsBody->setCategoryBitmask(TYPE_ENEMY);
        physicsBody->setGroup(GROUP_ENEMY);
        physicsBody->setContactTestBitmask(TYPE_BULLET | TYPE_PLAYER);
        physicsBody->setTag(TAG_BOSS);
        bossPart->setPhysicsBody(physicsBody);
        bosses.pushBack(bossPart->getPhysicsBody());
        addChild(bossPart, LAYER_GAME);
        bossHealth[i] = 2;
    }
    this->schedule(schedule_selector(GameScene::updateBoss), 0.8f, kRepeatForever, 0.0f);
}

void GameScene::fire() {
    auto bullet = Sprite::create("bullet.png");
    bullet->setPosition(player->getPosition().x,
        player->getPosition().y + player->getContentSize().height / 2);
    bullet->setAnchorPoint(Vec2(0.5, 1.0));

    auto physicsBody = PhysicsBody::createBox(
        Size(bullet->getContentSize().width / 4, bullet->getContentSize().height / 4));

    physicsBody->setRotationEnable(false);

    physicsBody->setVelocity(Vec2(0, SPEED_BULLET));

    physicsBody->setCategoryBitmask(TYPE_BULLET);
    physicsBody->setGroup(GROUP_PLAYER);
    physicsBody->setContactTestBitmask(TYPE_ENEMY | TYPE_BULLET | TYPE_EDGE);
    physicsBody->setTag(TAG_BULLET);

    bullet->setPhysicsBody(physicsBody);
    addChild(bullet, LAYER_GAME);

    /* 音效 */
    auto audio = SimpleAudioEngine::getInstance();
    audio->playEffect("music/fire.wav", false, 1.0f, 1.0f, 1.0f);
}

void GameScene::hurt(int8 tag) {
    /* 掉血 */
    const float health = tag * 0.5; /* 血量减少百分比 */
    auto percentage = pT->getPercentage() - health;
    if (percentage < 0) {
        /* 动画 */
        pT->runAction(ProgressTo::create(0.2f, 0));
        end(false);
    } else {
        pT->runAction(ProgressTo::create(0.2f, percentage));
    }
}
