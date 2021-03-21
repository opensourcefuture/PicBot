#include "websocket.h"
#include <iostream>
#include <thread>
#include <chrono>

#ifdef __GNUC__
#define __declspec(dllexport)
#endif

extern "C"
{
    static WebSocket * client = nullptr;
    void __declspec(dllexport) Bot_Connect(const char * ws_url,void((*callback)(const char *,void *)),void * funData)
    {
        static int tm = 0;
        if(tm != 0)
        {
            return ;
        }
        if(tm == 0)
        {
            tm = 1;
        }
        client = WebSocket::GetInstance();

        client->SetUrl(ws_url);
        std::thread([=]()
        {
            while(true)
            {
                
                if(!client->IsConnected())
                {
                    client->Connect();
                }
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }).detach();
        std::thread([callback,funData]()
        {
            try
            {
                while (true)
            {
                std::string msg;
                if(client->GetMessage(msg) == 0)
                {
                    if(callback)
                    {
                        callback(msg.c_str(),funData);
                    }
                }else
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(2));
                }
            }
            }
            catch(...)
            {
                std::cerr << "unkonw error GetMessage" << '\n';
            }
            
            
        }).detach();
    }
    void __declspec(dllexport) Bot_Send(const char * msg)
    {
        if(client && msg)
        {
            client->SendMsg(msg);
        }
    }
    int __declspec(dllexport) Bot_IsConnect()
    {
        if(!client)
        {
            return -1;
        }
        if(client->IsConnected())
        {
            return 0;
        }
        return -2;
    }
}

// static void RespFun(const char * msg,void * funData)
// {
//     std::cout << msg << std::endl;
    
// }

// int main()
// {
//     //"ws://localhost:6700"
//     Bot_Connect("ws://localhost:6700",RespFun,0);
//     while(1)
//     {
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//         std::string mssg = R"({"echo","11111""action":"send_group_msg","params":{"group_id":588093809,"message":"Hello"}})";
//         Bot_Send(mssg.c_str());
//     }
//     return 0;
// }




