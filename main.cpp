#include "OneBotBase.h"

#include <thread>
#include <chrono>
#include <iostream>
#include "PicGet.h"

using namespace std;
using json = nlohmann::json;

int main()
{
    OneBotBase::GetInstance("ws://127.0.0.1:6700", [](OneBotBase *bot, const json &j) {
        if (j["post_type"].get<string>() == "message" && j["message_type"].get<string>() == "group")
        {
            std::string pic = GetImgCQ(j["message"].get<string>());
            if (pic != "")
            {
                std::cout << pic << std::endl;
                json retJson = bot->Bot_SendApi(json({{"action", "send_group_msg"}, {"params", {{"group_id", j["group_id"].get<uint64_t>()}, {"message", pic}, {"auto_escape", false}}}}));
                int64_t id = retJson["data"]["message_id"].get<int64_t>();
                std::this_thread::sleep_for(std::chrono::seconds(30));
                bot->Bot_SendApi(json({{"action", "delete_msg"}, {"params", {{"message_id", id}}}}));
            }
        }
    });
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}
