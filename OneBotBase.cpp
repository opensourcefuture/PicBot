
#include "OneBotBase.h"

#include "EasyWST.h"
#include <iostream>
#include <list>
#include <mutex>
#include <random>
#include <ctime>
#include <chrono>
#include <thread>

#define BUFF_MAX_SIZE 1024

class OneBotBaseImpl:public OneBotBase
{
public:
    nlohmann::json Bot_SendApi(const nlohmann::json &j, int timeout);
    OneBotBaseImpl(const std::string &ws_url, void (*eventFun)(OneBotBase *, const nlohmann::json &));
private:
    static std::list<nlohmann::json> apiBackList;
    static std::mutex mtxApi;
    static std::list<nlohmann::json> eventList;
    static std::mutex mtxEvent;
    static std::list<void (*)(const nlohmann::json &)> funList;
    static std::string GenRandStr();
    void (*eventFun)(OneBotBase *, const nlohmann::json &);
    static void RespFun(const char *msg, void *funData);
};

std::list<nlohmann::json> OneBotBaseImpl::apiBackList;
std::mutex OneBotBaseImpl::mtxApi;
std::list<nlohmann::json> OneBotBaseImpl::eventList;
std::mutex OneBotBaseImpl::mtxEvent;

OneBotBaseImpl::OneBotBaseImpl(const std::string &ws_url, void (*eventFun)(OneBotBase *, const nlohmann::json &))
{
    Bot_Connect(ws_url.c_str(), RespFun, 0);
    this->eventFun = eventFun;
    std::thread([this]() {
        while (1)
        {
            nlohmann::json j;
            {
                std::lock_guard<std::mutex> lck(mtxEvent);
                if (eventList.size() > 0)
                {
                    j = eventList.front();
                    eventList.pop_front();
                }
            }
            if (!j.is_null())
            {
                std::thread([j, this]() {
                    try
                    {
                        this->eventFun(this, j);
                    }
                    catch (const std::exception &e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                }).detach();
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }
        }
    }).detach();
}

nlohmann::json OneBotBaseImpl::Bot_SendApi(const nlohmann::json &j, int timeout)
{
    if (Bot_IsConnect() != 0)
    {
        throw std::runtime_error("Bot_SendApi:Bot not connected");
    }
    nlohmann::json j_t = j;
    std::string uuid = GenRandStr();
    j_t["echo"] = uuid;
    time_t startTime = time(0);
    Bot_Send(j_t.dump().c_str());
    for (;;)
    {
        {
            std::lock_guard<std::mutex> lck(mtxApi);
            for (std::list<nlohmann::json>::iterator it = apiBackList.begin(); it != apiBackList.end(); ++it)
            {
                if ((*it)["echo"].get<std::string>() == uuid)
                {
                    nlohmann::json retJson = (*it);
                    apiBackList.erase(it);
                    return retJson;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (time(NULL) - startTime > timeout)
        {
            break;
        }
    }
    throw std::runtime_error("Bot_SendApi:error,timeout");
}

OneBotBase *OneBotBase::GetInstance(const std::string &ws_url, void (*eventFun)(OneBotBase *, const nlohmann::json &))
{
    static OneBotBase *bot = new OneBotBaseImpl(ws_url, eventFun);
    return bot;
}

std::string OneBotBaseImpl::GenRandStr()
{
    static std::default_random_engine random;
    std::string retStr;
    for (int i = 0; i < 20; ++i)
    {
        retStr += (random() % 26 + 'A');
    }
    return retStr;
}

void OneBotBaseImpl::RespFun(const char *msg, void *funData)
{
    if (msg)
    {
        try
        {
            nlohmann::json j = nlohmann::json::parse(msg);
            try
            {
                if (j["post_type"].is_null())
                {
                    throw std::runtime_error("No such key");
                }
                std::lock_guard<std::mutex> lck(mtxEvent);
                eventList.emplace_back(j);
                if (eventList.size() > BUFF_MAX_SIZE)
                {
                    size_t popSize = eventList.size() - BUFF_MAX_SIZE;
                    for (size_t i = 0; i < popSize; ++i)
                    {
                        eventList.pop_front();
                    }
                }
            }
            catch (const std::exception &e)
            {
                try
                {

                    if (j["status"].is_null() || j["echo"].is_null())
                    {
                        throw std::runtime_error("No such key");
                    };
                    std::lock_guard<std::mutex> lck(mtxApi);
                    apiBackList.emplace_back(j);
                    if (apiBackList.size() > BUFF_MAX_SIZE)
                    {
                        size_t popSize = apiBackList.size() - BUFF_MAX_SIZE;
                        for (size_t i = 0; i < popSize; ++i)
                        {
                            apiBackList.pop_front();
                        }
                    }
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                    std::cerr << msg << '\n';
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            std::cerr << msg << '\n';
        }
    }
}
