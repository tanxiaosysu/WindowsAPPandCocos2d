#include "Thunder.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;

#define DIS_EVERYTIME 1 /* ÿ���ƶ��ľ��� */
#define LAYER_BG 0
#define LAYER_PLAY 1
#define MIN_DISTANCE 30

Scene* Thunder::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = Thunder::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool Thunder::init() {
    if (!Layer::init()) {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();

    auto bgsprite = Sprite::create("background.jpg");
    bgsprite->setPosition(visibleSize / 2);
    // bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width, \
        //     visibleSize.height / bgsprite->getContentSize().height);
    this->addChild(bgsprite, LAYER_BG);

    player = Sprite::create("player.png");
    player->setPosition(visibleSize.width / 2, player->getContentSize().height + 5);
    player->setName("player");
    this->addChild(player, LAYER_PLAY);

    addEnemy(5);

    preloadMusic();
    playBgm();

    addTouchListener();
    addKeyboardListener();
    addCustomListener();

    // TODO
    // add schedule
    schedule(schedule_selector(Thunder::update), 0.02f, kRepeatForever, 0);

    return true;
}

void Thunder::preloadMusic() {
    // TODO
    /* Ԥ�� */
    auto audio = SimpleAudioEngine::getInstance();
    audio->preloadBackgroundMusic("music/bgm.mp3");
    audio->preloadEffect("music/fire.wav");
}

void Thunder::playBgm() {
    // TODO
    /* BGM */
    auto audio = SimpleAudioEngine::getInstance();
    audio->playBackgroundMusic("music/bgm.mp3", true);
}

void Thunder::addEnemy(int n) {
    enemys.resize(n * 3);
    for (unsigned i = 0; i < 3; ++i) {
        char enemyPath[20];
        sprintf(enemyPath, "stone%d.png", 3 - i);
        double width = (visibleSize.width - 20) / (n + 1.0),
            height = visibleSize.height - (50 * (i + 1));
        for (int j = 0; j < n; ++j) {
            auto enemy = Sprite::create(enemyPath);
            enemy->setAnchorPoint(Vec2(0.5, 0.5));
            enemy->setScale(0.5, 0.5);
            enemy->setPosition(width * (j + 1), height);
            enemys[i * n + j] = enemy;
            addChild(enemy);
        }
    }
}

void Thunder::addTouchListener() {
    // 
}

void Thunder::addKeyboardListener() {
    // TODO

    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void Thunder::update(float f) {
    // TODO
    static double count = 0;
    static int dir = 1;
    count += f;
    if (count > 1) {
        count = 0.0; dir = -dir;
    }

    if (!bullets.empty()) { /* ��Ļ�����ӵ� */
        /* ���������ӵ������� */
        auto itB = bullets.begin();
        while (itB != bullets.end()) {
            (*itB)->setPosition((*itB)->getPositionX(), (*itB)->getPositionY() + 5);
            if ((*itB)->getPositionY() > visibleSize.height - 10) {
                /* ������Ļ���ӵ���Ҫ���Ƴ� */
                (*itB)->removeFromParentAndCleanup(true);
                itB = bullets.erase(itB);
            } else {
                itB++;
            }
        }
    }

    /* ˫��ѭ���������е�enemy��bullet */
    for (unsigned i = 0; i < enemys.size(); i++) {
        if (enemys[i] != NULL) {
            enemys[i]->setPosition(enemys[i]->getPosition() + Vec2(dir, 0));

            if (bullets.empty()) {
                continue;
            }
            auto itB = bullets.begin();
            while (itB != bullets.end()) {
                if ((*itB)->getPosition().getDistance(enemys[i]->getPosition()) < MIN_DISTANCE) {
                    /* ��ײ��Ҫ�����¼� */
                    EventCustom e("meet");
                    e.setUserData(&i);
                    _eventDispatcher->dispatchEvent(&e);

                    /* �Ƴ��ӵ� */
                    (*itB)->removeFromParentAndCleanup(true);
                    itB = bullets.erase(itB);
                    break;
                } else {
                    itB++;
                }
            }
        }
    }

    player->setPosition(player->getPosition() + Vec2(move, 0));
}

void Thunder::fire() {
    /* ÿ�ο���, �ӵ���Vector����β������һ���µĳ�Ա */
    auto bullet = Sprite::create("bullet.png");
    bullet->setPosition(player->getPosition());
    bullets.push_back(bullet);
    addChild(bullets.back());

    /* ��Ч */
    auto audio = SimpleAudioEngine::getInstance();
    audio->playEffect("music/fire.wav", false, 1.0f, 1.0f, 1.0f);
}

void Thunder::meet(EventCustom * event) {
    /* ��������ĵ���, �����Ϻ���Ļ�� */
    int* data = (int*)(event->getUserData());
    enemys[*data]->removeFromParentAndCleanup(true);
    enemys[*data] = NULL;
}

void Thunder::addCustomListener() {
    // TODO
    auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));

    _eventDispatcher->addEventListenerWithFixedPriority(meetListener, 1);
}

bool Thunder::onTouchBegan(Touch *touch, Event *unused_event) {
    //
    return true;
}

void Thunder::onTouchMoved(Touch *touch, Event *unused_event) {
    //
}

void Thunder::onTouchEnded(Touch *touch, Event *unused_event) {
    //
}

void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
    auto pos = player->getPosition();
    switch (code) {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
        // TODO
        player->setPosition(pos.x - DIS_EVERYTIME, pos.y);
        move -= 5;
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
        // TODO
        player->setPosition(pos.x + DIS_EVERYTIME, pos.y);
        move += 5;
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
        fire();
        break;
    default:
        break;
    }
}

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
    // TODO
    auto pos = player->getPosition();
    switch (code) {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
        player->setPosition(pos.x - DIS_EVERYTIME, pos.y);
        move += 5;
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
        // TODO
        player->setPosition(pos.x + DIS_EVERYTIME, pos.y);
        move -= 5;
        break;
    default:
        break;
    }
}
