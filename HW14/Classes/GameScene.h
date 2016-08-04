#pragma once

#include "Global.h"

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "network/HttpClient.h"

#include <string>

using std::string;

using namespace cocos2d::ui;
using namespace cocos2d::network;

class GameScene : public cocos2d::Layer {
    public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);

    private:
    /* submit按钮点击事件. */
    void submitClick(Ref * pSender);
    /* submit按钮回调函数. */
    void submitCallback(HttpClient * sender, HttpResponse * response);

    /* rank按钮点击事件. */
    void rankClick(Ref * pSender);
    /* rank按钮回调函数. */
    void rankCallback(HttpClient * sender, HttpResponse * response);

    float visibleHeight;
    float visibleWidth;
    TextField * score_field;
    TextField * rank_field;
    Button *submit_button;
    Button *rank_button;
};
