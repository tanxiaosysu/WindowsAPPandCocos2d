#pragma once

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

#define ENEMY_COUNT 38
#define BOSS_PART_COUNT 5

class GameScene :public Layer {
    public:
    void setPhysicsWorld(PhysicsWorld * world);
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init(PhysicsWorld* world);

    // implement the "static create()" method manually
    static GameScene* create(PhysicsWorld* world);

    private:
    // 音乐加载与播放
    Sprite* player;
    Sprite* backGround0;
    Sprite* backGround1;
    PhysicsWorld* m_world;
    Size visibleSize;
    Vector<PhysicsBody*> enemys;
    Vector<PhysicsBody*> bosses;
    uint16 enemysHealth[ENEMY_COUNT];
    uint16 bossHealth[BOSS_PART_COUNT];

    // 
    cocos2d::Label* timeLabel;
    cocos2d::Label* scoreLabel;
    int time;
    cocos2d::ProgressTimer* pT;

    void preloadMusic();
    void playBgm();

    void addBackground();
    void addEdge();
    void addPlayer();
    void addUI();

    void addKeyboardListener();
    void addContactListener();

    void onKeyPressed(EventKeyboard::KeyCode code, Event* event);
    void onKeyReleased(EventKeyboard::KeyCode code, Event* event);
    bool onConcactBegan(PhysicsContact& contact);

    void updatePerSecond(float f);
    void updateBackground(float f);
    void updateEnemy(float f);
    void updateEnemyPerWave();
    void createBoss(float f);
    void updateBoss(float f);
    void end(bool ifWin);

    void newEnemys(int8 wave);
    void addEnemy(int type, Point p);
    void addBoss();
    void fire();
    void hurt(int8 tag);
};
