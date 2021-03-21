#ifndef __WEBSOCKET_H__
#define __WEBSOCKET_H__
#include <string>
#include <memory>

class WebSocket
{
public:
    virtual int32_t GetMessage(std::string & msg) = 0;
    virtual int32_t SendMsg(const std::string & msg) = 0;
    virtual int32_t Connect() = 0;
    virtual int32_t DisConnect() = 0;
    virtual bool IsConnected() = 0;
    virtual int32_t SetUrl(const std::string & msg) = 0;
    static WebSocket * GetInstance();
};

#endif
