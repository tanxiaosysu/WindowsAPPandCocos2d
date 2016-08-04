#include "GameScene.h"

#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

#include <regex>

using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;

using namespace rapidjson;

USING_NS_CC;

cocos2d::Scene* GameScene::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool GameScene::init() {
    if (!Layer::init()) {
        return false;
    }

    Size size = Director::getInstance()->getVisibleSize();
    visibleHeight = size.height;
    visibleWidth = size.width;

    score_field = TextField::create("Score", "Arial", 30);
    score_field->setPosition(Size(visibleWidth / 4, visibleHeight / 4 * 3));
    this->addChild(score_field, 2);

    submit_button = Button::create();
    submit_button->setTitleText("Submit");
    submit_button->setTitleFontSize(30);
    submit_button->setPosition(Size(visibleWidth / 4, visibleHeight / 4));
    submit_button->addClickEventListener(CC_CALLBACK_1(GameScene::submitClick, this));
    this->addChild(submit_button, 2);

    rank_field = TextField::create("", "Arial", 30);
    rank_field->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4 * 3));
    this->addChild(rank_field, 2);

    rank_button = Button::create();
    rank_button->setTitleText("Rank");
    rank_button->setTitleFontSize(30);
    rank_button->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4));
    rank_button->addClickEventListener(CC_CALLBACK_1(GameScene::rankClick, this));
    this->addChild(rank_button, 2);

    return true;
}

void GameScene::submitClick(Ref * pSender) {
    if (score_field->getString().empty()) { /* 分数非空 */
        return;
    }
    HttpRequest* request = new HttpRequest(); /* 创建连接 */
    request->setRequestType(HttpRequest::Type::POST); /* 设置POST方法 */
    const string url = Global::baseURL + REQ_TYPE_SUBMIT;
    request->setUrl(url.c_str()); /* 设置URL */
    /* 设置HEADER */
    vector<string> headers;
        headers.push_back("Cookie: GAMESESSIONID=" + Global::gameSessionId);
    request->setHeaders(headers);
    /* 设置参数 */
    const string reqWord = "score=" + score_field->getString();
    request->setRequestData(reqWord.c_str(), reqWord.size());
    /* 设置回调函数 */
    request->setResponseCallback(CC_CALLBACK_2(GameScene::submitCallback, this));

    cocos2d::network::HttpClient::getInstance()->send(request); /* 发送请求 */
    request->release(); /* 释放连接 */
}

void GameScene::submitCallback(HttpClient * sender, HttpResponse * response) {
    if (!response) {
        return;
    }
    if (!(response->isSucceed())) { /* 失败处理 */
        CCLOG("response failed");
        CCLOG("error buffer: %s", response->getErrorBuffer());
        return;
    }

    /* request data */
    auto buffer = response->getResponseData();
    string responseData = Global::toString(buffer);
    CCLOG("submit data:\n%s\n", responseData.c_str());
}

void GameScene::rankClick(Ref * pSender) {
    HttpRequest* request = new HttpRequest(); /* 创建连接 */
    request->setRequestType(HttpRequest::Type::GET); /* 设置GET方法 */
    const string url = Global::baseURL + REQ_TYPE_RANK + "?top=10";
    request->setUrl(url.c_str()); /* 设置URL */
    /* 设置HEADER */
    vector<string> headers;
    headers.push_back("Cookie: GAMESESSIONID=" + Global::gameSessionId);
    request->setHeaders(headers);
    /* 设置回调函数 */
    request->setResponseCallback(CC_CALLBACK_2(GameScene::rankCallback, this));

    cocos2d::network::HttpClient::getInstance()->send(request); /* 发送请求 */
    request->release(); /* 释放连接 */
}

void GameScene::rankCallback(HttpClient * sender, HttpResponse * response) {
    if (!response) {
        return;
    }
    if (!(response->isSucceed())) { /* 失败处理 */
        CCLOG("response failed");
        CCLOG("error buffer: %s", response->getErrorBuffer());
        return;
    }

    /* request data */
    auto buffer = response->getResponseData();
    string responseData = Global::toString(buffer);
    CCLOG("rank data:\n%s\n", responseData.c_str());

    /* 解析返回信息 */
    auto result = Global::getValueFromJson(responseData, "result");
    if (true == (bool)(result)) {
        /* 获取返回字符串 */
        auto info = Global::getValueFromJson(responseData, "info");
        string* tempStr = static_cast<string*>(info);
        string infoStr = *tempStr;
        delete tempStr;
        if (infoStr.size() > 2) {
            const char trimStr = '|';
            /* 掐头去尾 */
            infoStr.erase(infoStr.find_last_not_of(trimStr) + 1);
            infoStr.erase(0, infoStr.find_first_not_of(trimStr));
            /* 替换中间 */
            for (int i = 0; i < infoStr.size(); i++) {
                if (infoStr[i] == trimStr) {
                    infoStr[i] = '\n';
                }
            }
            /* 显示 */
            rank_field->setString(infoStr);
        }
    }
}
