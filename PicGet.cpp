#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "./httplib/httplib.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include <time.h>

using namespace std;

static std::vector<std::string> GetUrl(const std::string &body)
{
    std::vector<std::string> retVec;
    for (size_t i = 0; i < body.length(); ++i)
    {
        size_t pos = body.find("\"objURL\":\"", i);
        if (pos != body.npos)
        {
            i = pos + 10;
            size_t pos2 = body.find("\"", i);
            if (pos2 != body.npos)
            {
                retVec.emplace_back(body.substr(i, pos2 - i));
            }
        }
    }
    return retVec;
}

static std::string GetHtml(const std::string &keyword)
{
    httplib::Headers headers = {
        {"User-Agent", "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.72 Safari/537.36"},
        {"Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"}};
    httplib::Client cli("https://image.baidu.com");
    cli.enable_server_certificate_verification(false);
    auto res = cli.Get(("/search/index?tn=baiduimage&word=" + keyword).c_str(), headers);
    if (!res)
    {
        printf("res error code:%d\n", res.error());
        return "";
    }
    if (res->status != 200)
    {
        return "";
    }
    printf("res->body:%d", res->body.length());
    return res->body;
}

static std::string &replace_all_distinct(std::string &str, const std::string &old_value, const std::string &new_value)
{
    for (string::size_type pos(0); pos != string::npos; pos += new_value.length())
    {
        if ((pos = str.find(old_value, pos)) != string::npos)
            str.replace(pos, old_value.length(), new_value);
        else
            break;
    }
    return str;
}

static string GetKeyWord(std::string input)
{
    replace_all_distinct(input, "&amp;", "&");
    replace_all_distinct(input, "&#91;", "[");
    replace_all_distinct(input, "&#93;", "]");
    replace_all_distinct(input, "&#44;", ",");
    auto pos1 = input.find("来点");
    auto len1 = string("来点").length();
    auto len2 = string("的色图").length();
    auto pos2 = input.find("的色图");
    if (pos1 == 0 && pos2 + len2 == input.length())
    {
        return input.substr(pos1 + len1, pos2 - pos1 - len1);
    }
    return "";
}

std::string GetImgCQ(const std::string &input)
{
    auto keyword = GetKeyWord(input);
    if (keyword == "")
    {
        return "";
    }
    printf("key:#%s#\n", keyword.c_str());
    auto htmlStr = GetHtml(keyword);
    printf("htmlLen:%d\n", htmlStr.length());
    auto vec = GetUrl(htmlStr);
    printf("urlVec:%d\n", vec.size());
    if (vec.size() > 0)
    {
        std::string url = vec[time(0) % vec.size()];
        replace_all_distinct(url, "&", "&amp;");
        replace_all_distinct(url, "[", "&#91;");
        replace_all_distinct(url, "]", "&#93;");
        replace_all_distinct(url, ",", "&#44;");
        std::string pic = "[CQ:image,file=" + url + "]";
        return pic;
    }
    return "";
}
