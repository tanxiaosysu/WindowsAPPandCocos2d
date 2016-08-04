#include "Global.h"
#include "LoginScene.h"
#include "GameScene.h"

#include "cocostudio/CocoStudio.h"

#include <regex>

using std::to_string;
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;

USING_NS_CC;
using namespace cocostudio::timeline;

Scene* LoginScene::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = LoginScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool LoginScene::init() {
    // 1. super init first
    if (!Layer::init()) {
        return false;
    }

    Size size = Director::getInstance()->getVisibleSize();
    visibleHeight = size.height;
    visibleWidth = size.width;

    textField = TextField::create("Type Your Name", "Arial", 30);
    textField->setPosition(Size(visibleWidth / 2, visibleHeight / 4 * 3));
    this->addChild(textField, 2);

    auto button = Button::create();
    button->setTitleText("Start");
    button->setTitleFontSize(30);
    button->setPosition(Size(visibleWidth / 2, visibleHeight / 2));
    button->addClickEventListener(CC_CALLBACK_1(LoginScene::loginClick, this));
    this->addChild(button, 2);

    return true;
}

void LoginScene::loginClick(Ref * pSender) {
    if (textField->getString().empty()) { /* 用户名非空 */
        return;
    }
    HttpRequest* request = new HttpRequest(); /* 创建连接 */
    request->setRequestType(HttpRequest::Type::POST); /* 设置POST方法 */
    const string url = Global::BASE_URL + Global::REQ_TYPE_LOGIN;
    request->setUrl(url.c_str()); /* 设置URL */
    std::vector<std::string> headers;
    headers.push_back("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
    request->setHeaders(headers);
    const string reqWord = "username=" + textField->getString();
    request->setRequestData(reqWord.c_str(), reqWord.size()); /* 设置参数 */
    /* 设置回调函数 */
    request->setResponseCallback(CC_CALLBACK_2(LoginScene::loginCallback, this));

    cocos2d::network::HttpClient::getInstance()->send(request); /* 发送请求 */
    request->release(); /* 释放连接 */
}

/* 登陆回调 */
void LoginScene::loginCallback(HttpClient * sender, HttpResponse * response) {
    if (!response) {
        return;
    }
    if (!(response->isSucceed())) { /* 失败处理 */
        CCLOG("response failed");
        CCLOG("error buffer: %s", response->getErrorBuffer());
        return;
    }

    /* request header */
    auto buffer = response->getResponseHeader();
    string responseHeader = Global::toString(buffer);
    log("login header:\n%s", responseHeader.c_str());

    /* request data */
    buffer = response->getResponseData();
    string responseData = Global::toString(buffer);
    log("login data:\n%s\n", responseData.c_str());

    /* 解析返回信息 */
    auto result = Global::getValueFromJson(responseData, "result");
    if (true == (bool)(result)) {
        Global::username = textField->getString(); /* 设置当前username */
        /* 设置sessionID */
        Global::gameSessionId = Global::getSessionIdFromHeader(responseHeader);
        switchScene();
    }
}

/* 场景切换 */
void LoginScene::switchScene() {
    this->unscheduleAllSelectors();
    _eventDispatcher->removeAllEventListeners();
    /* 创建游戏场景 */
    auto scene = GameScene::createScene();
    /* 切换(带渐隐效果) */
    Director::getInstance()->replaceScene(TransitionFade::create(0.1f, scene));
}
