#ifndef __ONEBOTBASE_H__
#define __ONEBOTBASE_H__



#include "nlohmann/json.hpp"
#include <string>

class OneBotBase
{
public:
    virtual nlohmann::json Bot_SendApi(const nlohmann::json &j, int timeout = 30) = 0;
    static OneBotBase *GetInstance(const std::string &ws_url, void (*eventFun)(OneBotBase *, const nlohmann::json &));   
};


#endif
