#ifndef __SPLITTOOLCPPJIEBA_H__
#define __SPLITTOOLCPPJIEBA_H__

#include "SplitTool.h"
#include "./Configuration_jieba.h"
#include "./cppjieba/Jieba.hpp"

class SplitToolCppJieba
:public SplitTool
{
public:
    SplitToolCppJieba(Configuration_jieba conf)
    :_conf(conf)
    ,_map(_conf.getConfigMap())
    ,_jieba(_map["DICT_PATH"].c_str()
            ,_map["HMM_PATH"].c_str()
            ,_map["USER_DICT_PATH"].c_str())
    {
    }

    virtual ~SplitToolCppJieba(){}
    vector<string> cut(const string &sentence);
    map<string, int> getWordFrequency(string str);

private:
    Configuration_jieba _conf;       //配置文件
    map<string, string> _map;   //用来初始化jieba
    cppjieba::Jieba _jieba;     //用来调用jieba中的Cut函数
};
//分词，存入vector中
inline vector<string> SplitToolCppJieba::cut(const string &sentence)
{
    vector<string> words;
    _jieba.Cut(sentence, words, true);
    return words;
}
//将string分词，并获取词频
inline map<string, int> SplitToolCppJieba::getWordFrequency(string str)
{
    vector<string> words;
    map<string, int> mp;
    //读取停用词
    unordered_set<string> stopWordList = _conf.getStopWordList();
    //分词，并存入vector中
    _jieba.Cut(str, words, true); 
    //遍历vector
    for(auto &word : words)
    {
        auto iter = stopWordList.find(word);    //查看一下处理完的单词是否是停用词
        if(iter != stopWordList.end())          //是停用词，则清除
        {
            word.clear();
            continue ;
        }
        //去除英文字符、数字、标点、空格
        for(auto &ch : word)
        {
            if(isalpha(ch) || isdigit(ch) || ispunct(ch) || isspace(ch) )
            {
                word.clear();
                continue;
            }
        }
        //单词非空，则统计词频
        if(!word.empty() )
            ++mp[word];
    }
    return mp;
}

#endif