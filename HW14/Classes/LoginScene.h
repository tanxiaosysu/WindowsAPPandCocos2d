#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "ui/CocosGUI.h"
#include "network/HttpClient.h"
#include "cocos2d.h"

#include <string>
#include <vector>

using std::vector;
using std::string;

using namespace cocos2d::ui;
using namespace cocos2d::network;

class LoginScene : public cocos2d::Layer {
    public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(LoginScene);

    private:
    /* login按钮点击事件. */
    void loginClick(Ref * pSender);
    /* login按钮回调函数. */
    void loginCallback(HttpClient * sender, HttpResponse * response);

    /* 切换场景 */
    void switchScene();

    float visibleHeight;
    float visibleWidth;
    TextField * textField;
};

#endif // __HELLOWORLD_SCENE_H__
