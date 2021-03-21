# PicBot
### 介绍
QQ机器人百度图片搜索，随机展示
### 构建
#### 构建依赖
* openssl-1.1.1<br>
* c++11及其以上，注意CentOS7自带的gcc4.8.5不行<br>
#### 大致的构建命令
##### win构建(Win7x64):
<code>g++ *.cpp LightWebSocketClient\*.cpp -I "C:\Program Files\OpenSSL-Win64\include" "C:\Program Files\OpenSSL-Win64\lib\libssl.lib" "C:\Program Files\OpenSSL-Win64\lib\libcrypto.lib" -lws2_32 -lcrypt32 -std=c++11</code>
##### linux构建(CentOS7x64):
<code>g++ *.cpp LightWebSocketClient/*.cpp -I openssl-1.1.1d/include openssl-1.1.1d/libssl.a openssl-1.1.1d/libcrypto.a -lpthread -lz -ldl -std=c++11</code>
### 运行
#### 运行依赖
实现了 OneBot V11 的框架，开启websocket正向链接
#### 触发方式
来点XXX的色图
### 相关项目
https://github.com/howmanybots/onebot <br>
https://github.com/cyanray/LightWebSocketClient <br>
https://github.com/nlohmann/json <br>
https://github.com/yhirose/cpp-httplib <br>
https://www.openssl.org/ <br>
https://github.com/mamoe/mirai <br>
https://github.com/Mrs4s/go-cqhttp <br>
https://github.com/yyuueexxiinngg/onebot-kotlin <br>
