#include"Monster.h"
USING_NS_CC;

Factory* Factory::factory = NULL;
Factory::Factory() {
    initSpriteFrame();
}
Factory* Factory::getInstance() {
    if (factory == NULL) {
        factory = new Factory();
    }
    return factory;
}
void Factory::initSpriteFrame() {
    auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
    monsterDead.reserve(4);
    for (int i = 0; i < 4; i++) {
        auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258 - 48 * i, 0, 42, 42)));
        monsterDead.pushBack(frame);
    }
}

Sprite* Factory::createMonster() {
    Sprite* mons = Sprite::create("Monster.png", CC_RECT_PIXELS_TO_POINTS(Rect(364, 0, 42, 42)));
    monster.pushBack(mons);
    return mons;
}

void Factory::removeMonster(Sprite* sp) {
    auto it = monster.begin();
    while (it != monster.end()) {
        if (sp == (*it)) {
            /* 屏幕动画 */
            Animation * anm = Animation::createWithSpriteFrames(monsterDead, 0.05f);
            Animate * ani = Animate::create(anm);
            Sequence * seq = Sequence::create(
                ani, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sp)), NULL);
            sp->runAction(seq);
            /* 数据结构移除 */
            it = monster.erase(it);
        } else {
            it++;
        }
    }
}

void Factory::moveMonster(Vec2 playerPos, float time) {
    /* 遍历移动所有怪 */
    auto it = monster.begin();
    while (it != monster.end()) {
        Vec2 monsterPos = (*it)->getPosition();
        Vec2 direction = playerPos - monsterPos;
        direction.normalize();
        (*it)->runAction(MoveBy::create(time, direction * 20));
        it++;
    }
}

Sprite* Factory::collider(Rect rect) {
    /* 遍历检测碰撞 */
    auto it = monster.begin();
    while (it != monster.end()) {
        if (rect.containsPoint((*it)->getPosition())) {
            return (*it);
        } else {
            it++;
        }
    }
    return NULL;
}
