#include "Global.h"
#include "EndScene.h"
#include "GameScene.h"

#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

#include <regex>
#include <cstdlib>

using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;

using namespace rapidjson;

USING_NS_CC;

bool EndScene::ifWin = false;

cocos2d::Scene* EndScene::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = EndScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool EndScene::init() {
    if (!Layer::init()) {
        return false;
    }

    Size size = Director::getInstance()->getVisibleSize();
    visibleHeight = size.height;
    visibleWidth = size.width;

    score_field = TextField::create("", "Arial", 30);
    std::string scoreStr = "You ";
    scoreStr += (Global::ifWin ? "Win!\n" : "Lose!\n");
    scoreStr += "Score: " + Global::integerToString(Global::score) + '\n';
    /* 本地文件读取分数纪录 */
    int highscore = database->getIntegerForKey(Global::username.c_str());
    if (highscore > Global::score) {
        scoreStr += "Highscore: " + Global::integerToString(highscore) + '\n';
    } else {
        scoreStr += "Highscore: " + Global::integerToString(Global::score) + '\n';
        database->setIntegerForKey(Global::username.c_str(), Global::score);
    }
    score_field->setString(scoreStr);
    score_field->setPosition(Size(visibleWidth / 4, visibleHeight / 4 * 3));
    this->addChild(score_field, 2);

    submit_button = Button::create();
    submit_button->setTitleText("Submit");
    submit_button->setTitleFontSize(30);
    submit_button->setPosition(Size(visibleWidth / 4, visibleHeight / 4));
    submit_button->addClickEventListener(CC_CALLBACK_1(EndScene::submitClick, this));
    this->addChild(submit_button, 2);

    rank_field = TextField::create("", "Arial", 30);
    rank_field->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4 * 3));
    this->addChild(rank_field, 2);

    rank_button = Button::create();
    rank_button->setTitleText("Rank");
    rank_button->setTitleFontSize(30);
    rank_button->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4));
    rank_button->addClickEventListener(CC_CALLBACK_1(EndScene::rankClick, this));
    this->addChild(rank_button, 2);

    retryButton = Button::create();
    retryButton->setTitleText("Retry");
    retryButton->setTitleFontSize(30);
    retryButton->setPosition(Size(visibleWidth / 2, retryButton->getContentSize().height));
    retryButton->addClickEventListener(CC_CALLBACK_1(EndScene::retryClick, this));
    this->addChild(retryButton, 2);
    return true;
}

void EndScene::submitClick(Ref * pSender) {
    //if (score_field->getString().empty()) { /* 分数非空 */
    //    return;
    //}
    HttpRequest* request = new HttpRequest(); /* 创建连接 */
    request->setRequestType(HttpRequest::Type::POST); /* 设置POST方法 */
    const string url = Global::BASE_URL + Global::REQ_TYPE_SUBMIT;
    request->setUrl(url.c_str()); /* 设置URL */
                                  /* 设置HEADER */
    vector<string> headers;
    headers.push_back("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
    headers.push_back("Cookie: GAMESESSIONID=" + Global::gameSessionId);
    request->setHeaders(headers);
    /* 设置参数 */
    std::string reqWord;
    /* 本地文件读取分数纪录 */
    int highscore = database->getIntegerForKey(Global::username.c_str());
    if (highscore > Global::score) {
        reqWord = "score=" + Global::integerToString(highscore);
    } else {
        reqWord = "score=" + Global::integerToString(Global::score);
    }
    //const string reqWord = "score=" + score_field->getString();
    //const string reqWord = "score=" + Global::integerToString(Global::score);
    request->setRequestData(reqWord.c_str(), reqWord.size());
    /* 设置回调函数 */
    request->setResponseCallback(CC_CALLBACK_2(EndScene::submitCallback, this));

    cocos2d::network::HttpClient::getInstance()->send(request); /* 发送请求 */
    request->release(); /* 释放连接 */
}

void EndScene::submitCallback(HttpClient * sender, HttpResponse * response) {
    if (!response) {
        return;
    }
    if (!(response->isSucceed())) { /* 失败处理 */
        log("response failed");
        log("error buffer: %s", response->getErrorBuffer());
        return;
    }

    /* request data */
    auto buffer = response->getResponseData();
    string responseData = Global::toString(buffer);
    log("submit data:\n%s\n", responseData.c_str());
}

void EndScene::rankClick(Ref * pSender) {
    HttpRequest* request = new HttpRequest(); /* 创建连接 */
    request->setRequestType(HttpRequest::Type::GET); /* 设置GET方法 */
    const string url = Global::BASE_URL + Global::REQ_TYPE_RANK + "?top=10";
    request->setUrl(url.c_str()); /* 设置URL */
                                  /* 设置HEADER */
    vector<string> headers;
    headers.push_back("Cookie: GAMESESSIONID=" + Global::gameSessionId);
    request->setHeaders(headers);
    /* 设置回调函数 */
    request->setResponseCallback(CC_CALLBACK_2(EndScene::rankCallback, this));

    cocos2d::network::HttpClient::getInstance()->send(request); /* 发送请求 */
    request->release(); /* 释放连接 */
}

void EndScene::rankCallback(HttpClient * sender, HttpResponse * response) {
    if (!response) {
        return;
    }
    if (!(response->isSucceed())) { /* 失败处理 */
        log("response failed");
        log("error buffer: %s", response->getErrorBuffer());
        return;
    }

    /* request data */
    auto buffer = response->getResponseData();
    string responseData = Global::toString(buffer);
    log("rank data:\n%s\n", responseData.c_str());

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

void EndScene::retryClick(Ref * pSender) {
    this->unscheduleAllSelectors();
    _eventDispatcher->removeAllEventListeners();
    Global::score = 0;
    Global::ifWin = false;
    /* 创建游戏场景 */
    auto scene = GameScene::createScene();
    /* 切换(带渐隐效果) */
    Director::getInstance()->replaceScene(TransitionFade::create(0.1f, scene));
}
