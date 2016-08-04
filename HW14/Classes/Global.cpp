#include "Global.h"

#include "json/rapidjson.h"
#include "json/document.h"

#include <regex>

using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;

string Global::baseURL = "http://localhost:8080/";
string Global::gameSessionId = "";
string Global::username = "";
long Global::score = 0;

Global::Global() {}

Global::~Global() {}

string Global::toString(vector<char> *buffer) {
    string rst;
    for (char ch : *buffer) {
        rst.push_back(ch);
    }
    return rst;
}

string Global::getSessionIdFromHeader(string head) {
    regex nlp("\\r\\n");
    string header = regex_replace(head, nlp, " ");
    regex pattern(".*GAMESESSIONID=(.*) Content-Type.*");
    //match_results<std::string::const_iterator> result;
    cmatch result;
    bool valid = regex_match(header.c_str(), result, pattern);

    if (!valid) {
        return "";
    }
    return result[1];
}

void* Global::getValueFromJson(const string & json, const string & name) {
    rapidjson::Document document;
    document.Parse(json.c_str());
    if (document.HasParseError()) {
        return NULL;
    }
    if (document.IsObject() && document.HasMember(name.c_str())) {
        auto result = document[name.c_str()].GetType(); /* 获取类型 */
        /* 返回指针 */
            int i = 20;
        switch (result) {
        case rapidjson::kTrueType:
        case rapidjson::kFalseType:
            return (void*)(document[name.c_str()].GetBool());
            break;
        case rapidjson::kStringType:
            return static_cast<void*>(new string(document[name.c_str()].GetString()));
            break;
        case rapidjson::kNumberType:
            return (void*)(document[name.c_str()].GetInt());
            break;
        default:
            break;
        }
    }
    return NULL;
}
