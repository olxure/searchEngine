#ifndef __SPLITTOOLCPPJIEBA_H__
#define __SPLITTOOLCPPJIEBA_H__

#include "../Configuration.h"
#include "./SplitTool.h"
#include "./cppjieba/Jieba.hpp"
#include "./NonCopyable.h"
#include <pthread.h>
#include <stdlib.h>
#include <utility>
#include <functional>

using std::bind;
using std::function;

//配合Conf（单例）使用，pthread_once + atexit + init单例模式
class SplitToolCppJieba
:public SplitTool
,public NonCopyable
{
public:
    static SplitToolCppJieba* getInstance(Configuration* conf)
    {
        if(_pInstance == nullptr)
        {
            _initFunc = bind(loadConfiguration, conf);
        }
        pthread_once(&_once, init);
        return _pInstance;
    }

    static void destroy()
    {
        if(_pInstance != nullptr)
        {
            delete _pInstance;
            _pInstance = nullptr;
        }
    }

    vector<string> cut(const string &sentence);     //分词
    map<string, int> getWordFrequency(string str);  //获取词频

private:
    SplitToolCppJieba(Configuration* conf)
    :_conf(conf)
    ,_map(_conf->getConfigMap())
    ,_jieba(_map["DICT_PATH"].c_str()
            ,_map["HMM_PATH"].c_str()
            ,_map["USER_DICT_PATH"].c_str())
    {
    }  

    static void init()
    {
        _initFunc();
        atexit(destroy);
    }
    //加载配置文件
    static void loadConfiguration(Configuration* conf)
    {
        _pInstance = new SplitToolCppJieba(conf);
    }
    
    virtual ~SplitToolCppJieba(){}

private:
    static SplitToolCppJieba * _pInstance;
    static function<void()> _initFunc;
    static pthread_once_t _once;
    Configuration* _conf;                    //配置文件
    map<string, string>& _map;               //用来初始化jieba
    cppjieba::Jieba _jieba;                  //用来调用jieba中的Cut函数
};

//分词，返回vector
inline vector<string> SplitToolCppJieba::cut(const string &sentence)
{
    //读取停用词
    unordered_set<string> stopWordList = _conf->getStopWordList();
    vector<string> words;
    //原生jieba的分词
    _jieba.Cut(sentence, words, true);
    //处理分词后的词语
    for(auto &word : words)
    {
        auto iter = stopWordList.find(word);    //查看一下处理完的单词是否是停用词
        if(iter != stopWordList.end())          //是停用词则清空
        {
            word.clear();
            continue ;
        }
        //取出标点和空格
        for(auto &ch : word)
        {
            if( ispunct(ch) || isspace(ch) )// ||isdigit(ch) || )
            {
                word.clear();
                continue;
            }
        }
    }
    //去除vector中的空字符串
    vector<string> newWords;
    for(auto &word : words)
    {
        if(!word.empty())
            newWords.push_back(word);
    }
    //也可以用words.erase(std::remove(words.begin(), words.end(), ""), words.end());
    return newWords;
}
//统计词频
inline map<string, int> SplitToolCppJieba::getWordFrequency(string str)
{
    vector<string> words;
    map<string, int> mp;
    unordered_set<string> stopWordList = _conf->getStopWordList();
    
    _jieba.Cut(str, words, true); 
    for(auto &word : words)
    {
        auto iter = stopWordList.find(word);    //查看一下处理完的单词是否是暂停词
        if(iter != stopWordList.end())          //是暂停词
        {
            word.clear();
            continue ;
        }
        
        for(auto &ch : word)
        {
            if(isalpha(ch) ||  ispunct(ch) || isspace(ch) )// ||isdigit(ch) || )
            {
                word.clear();
                continue;
            }
        }
        //字符串非空则统计词频
        if(!word.empty() )
            ++mp[word];
    }
    return mp;
}
//在.cc文件中初始化
/* SplitToolCppJieba * SplitToolCppJieba::_pInstance = nullptr;//懒汉/饱汉(懒加载)模式 */
/* pthread_once_t SplitToolCppJieba::_once = PTHREAD_ONCE_INIT; */
/* function<void()> SplitToolCppJieba::_initFunc; */

#endif
