#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <vector>

using std::string;
using std::vector;

/* 请求类型 */
#define REQ_TYPE_LOGIN "login"
#define REQ_TYPE_SUBMIT "submit"
#define REQ_TYPE_RANK "rank"

class Global {
    public:
    Global();
    ~Global();

    static string baseURL; /* 基本URL */
    static string gameSessionId;
    static string username; /* 当前用户名 */
    static long score;

    static string toString(vector<char> *buffer);

    static string getSessionIdFromHeader(string header);

    /**
    @brief  获取Json特定对象的value.
    @param  json: Json字符串
    @param  name: 对象的name
    @return void*: value的指针.
    */
    static void* getValueFromJson(const string & json = "", const string & name = "");
};

#endif
