#pragma once

#include "../include/Configuration.h"
#include "../include/NonCopyable.h"
#include "../include/WebPage.h"
#include <pthread.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <queue>
#include <string>
#include <utility>
#include <map>

using std::string;
using std::unordered_map;
using std::pair;
using std::vector;
using std::priority_queue;
using std::map;


// 结构体，多次包含结构体或者类是不会报错的
struct webPriority
{
    int id;
    double cosAngle;
    string word;
};

// 模板特化，相当于一个特殊的类
template<>
struct std::greater<webPriority>
{
    bool operator()(const webPriority& lhs, const webPriority& rhs)
    {
        if(lhs.cosAngle > rhs.cosAngle)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
};

class WebLib:public NonCopyable
{
public:
    static WebLib* getInstance(Configuration* conf);


    // wordList是分词完的一个个短语， candidateId是取交集完的文章id
    // word_TF_DF一个单词以及其TF和DF的值
    // candidateId和word_TF_DF是传入传出参数，传入时为空
    void doAndQuery(const vector<string>& wordList, vector<int>& candidateId, map<string, pair<int, int>>& word_TF_DF);

    // wordOccurCount是短语出现次数映射，candidateId是交集结果，queue是拍完优先级的文章id队列
    // queue是传入传出参数
    // 处理短语重复的向量，搞出一个新的向量
    void sortPages(const map<string, pair<int, int>>& word_TF_DF, const vector<int>& candidateId, priority_queue<webPriority, 
                        vector<webPriority>, std::greater<struct webPriority>>& queue);

    unordered_map<int, WebPage>& getWebPage();

private:
    WebLib();
    ~WebLib();

    // 把传入的短语数组作为一篇文章，算出对应的向量，这里并不考虑短语重复的情况
    map<string, double> queryVector(const map<string, pair<int, int>>& wordOccurCount);

    // 查找对应id的weight
    double findPageWeight(vector<pair<int, double>>& pageWeight, int id);

    // 余弦算法
    void cosAlgorithm(map<int, vector<double>>& pageVector, vector<double>& searchVector, string priorityWord,
                      priority_queue<webPriority, vector<webPriority>, std::greater<struct webPriority>>& queue);

    // 得到某个向量的绝对值
    double getAbsoluteValue(vector<double>& vec);

    // 求两个向量的点乘
    double crossVector(vector<double>& x, vector<double>& y);

private:
    // 下面是初始化单例类的私有函数,在多线程环境下只执行一次
    static void init();

    // 加载配置！！！
    static void loadConfiguration(Configuration* conf);

    static void destroy();

    void initOffsetLib(const string& filePath);
    void initInvertIndexLib(const string& filePath);
    void initWebPageLib(const string& filePath);

private:
    static WebLib * _pInstance;
    static pthread_once_t _once;
    static function<void()> _initFunc;

    unordered_map<int, pair<int,int>> _webOffsetLib;        //网页偏移库
    unordered_map<string, vector<pair<int, double>>> _invertIndexLib;   //倒排索引库
    unordered_map<int, WebPage> _webPageLib;        //网页库
};


