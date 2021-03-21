#include "websocket.h"
#include "LightWebSocketClient/WebSocketClient.h"

#include <mutex>
#include <list>
#include <thread>
#include <iostream>
#include <chrono>

using namespace std;
using namespace cyanray;

static void PrintLog(const std::string &msg)
{
    cout << msg << endl;
}

static std::pair<std::string,int>  GetHostAndPortFromUrl(const std::string & url)
{
    int pos1 = 0,pos2 = 0;
    for(int i = 0;i < (int)url.size();++i)
    {
        if(url.at(i) == ':')
        {
            if(pos1 == 0)
            {
                pos1 = i+3;
            }else
            {
                pos2 = i;
                break;
            }
            
        }
    }
    std::string host = url.substr(pos1,pos2 - pos1);
    int port = atoi(url.substr(pos2 + 1).c_str());
    return {host,port};
}


class WebSocketImpl : public WebSocket
{
public:
    WebSocketImpl()
    {
        std::lock_guard<std::mutex> lck(mtx);
        isConnect = false;
    }
    virtual int32_t GetMessage(std::string &msg)
    {

        std::lock_guard<std::mutex> lck(mtx);
        if (recvList.size() > 0)
        {
            msg = recvList.front();
            recvList.pop_front();
            return 0;
        }
        else
        {
            return -1;
        }
        return 0;
    }
    virtual int32_t SendMsg(const std::string &msg)
    {
        std::lock_guard<std::mutex> lck(mtx);
        sendList.emplace_back(msg);
        return 0;
    }
    virtual int32_t Connect()
    {
        std::lock_guard<std::mutex> lck(mtx);
        if (isConnect)
        {
            return -1;
        }
        if (url == "")
        {
            return -2;
        }
        try
        {
            std::pair<std::string,int> dat = GetHostAndPortFromUrl(url);
            client.Connect(dat.first,dat.second,"/");
            isConnect = true;
            client.OnTextReceived([&](WebSocketClient &client, string text) {
                std::lock_guard<std::mutex> lck(mtx);
                recvList.emplace_back(text);
            });
            client.OnLostConnection([&](WebSocketClient &client, int code) {
                std::lock_guard<std::mutex> lck(mtx);
                isConnect = false;
                PrintLog("Lost Connection......");
            });
            std::thread([&]() {
                try
                {
                    while (true)
                    {
                        bool isConnect_t;
                        {
                            std::lock_guard<std::mutex> lck(mtx);
                            isConnect_t = isConnect;
                        }
                        if (!isConnect_t)
                        {
                            break;
                        }

                        std::string sendMsg;
                        {
                            mtx.lock();
                            if (sendList.size() > 0)
                            {
                                sendMsg = sendList.front();
                                sendList.pop_front();
                                mtx.unlock();
                            }
                            else
                            {
                                mtx.unlock();
                                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                                continue;
                            }
                        }
                        try
                        {
                            client.SendText(sendMsg);
                        }
                        catch (const std::exception &ex)
                        {
                            PrintLog(ex.what());
                        }
                    }
                }
                catch (...)
                {
                    PrintLog("unkonw error");
                }
            }).detach();
        }
        catch (const std::exception &ex)
        {
            PrintLog(ex.what());
            isConnect = false;
            return -3;
        }
        catch (...)
        {
            PrintLog("unkonw error");
        }
        PrintLog("Connection Success......");
        return 0;
    }
    virtual int32_t DisConnect()
    {
        std::lock_guard<std::mutex> lck(mtx);
        try
        {
            client.Close();
        }
        catch (const std::exception &ex)
        {
            PrintLog(ex.what());
        }
        catch (...)
        {
            PrintLog("unkonw error");
        }
        isConnect = false;
        return 0;
    }
    virtual bool IsConnected()
    {
        std::lock_guard<std::mutex> lck(mtx);
        return isConnect;
    }
    virtual int32_t SetUrl(const std::string &url)
    {
        std::lock_guard<std::mutex> lck(mtx);
        this->url = url;
        return 0;
    }

private:
    std::string url;
    WebSocketClient client;
    bool isConnect;
    std::mutex mtx;
    std::list<std::string> sendList;
    std::list<std::string> recvList;
};

WebSocket * WebSocket::GetInstance()
{
    return new WebSocketImpl();
}
