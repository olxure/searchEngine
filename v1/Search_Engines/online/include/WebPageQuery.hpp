#pragma once

#include "WebPage.h"
#include "WebLib.hpp"
#include <unordered_map>
#include <utility>
#include <queue>
#include <vector>
#include <locale>
#include <codecvt>
#include <nlohmann/json.hpp>

using std::pair;
using std::unordered_map;
using std::priority_queue;
using std::vector;

using namespace nlohmann;

class WebPageQuery
{
public:
    WebPageQuery(Configuration* conf);
    ~WebPageQuery();

    // 对wordList做查询，结果放到_queue中
    void doQuery(vector<string>& wordList);

    // 对_queue中的网页进行摘要处理
    void handleAbstract();
    
    // 对生成摘要的网页进行处理，变成最终结果
    void generateJson();

    json getResult();

private:
    WebLib* _webLib;            // 网页库单例对象
    priority_queue<webPriority, vector<webPriority>, std::greater<struct webPriority>> _queue;  // 网页搜索的优先级队列
    vector<WebPage> _pages;     // 结果的网页
    json _finalResult;         // 最终封装的JSON对象
};

