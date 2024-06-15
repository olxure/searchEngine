#ifndef __SPLITTOOLCPPJIEBA_H__
#define __SPLITTOOLCPPJIEBA_H__

#include "./Configuration.h"
#include "./cppjieba/Jieba.hpp"
#include "./SplitTool.h"
#include <regex>

class SplitToolCppJieba
    : public SplitTool
{
public:
    SplitToolCppJieba(Configuration conf)
        : _conf(conf), _map(_conf.getConfigMap()), _jieba(_map["DICT_PATH"].c_str(), _map["HMM_PATH"].c_str(), _map["USER_DICT_PATH"].c_str(), _map["IDF_PATH"].c_str(), _map["STOP_WORD_PATH"].c_str())
    {
    }

    virtual ~SplitToolCppJieba() {}
    //切词，把string分解为词语，存储在vector<string>中并返回
    vector<string> cut(const string &sentence);
    //切词，并统计string中出现的词语的词频
    map<string, int> getWordFrequency(string str);

private:
    Configuration _conf;      // 配置文件
    map<string, string> _map; // 用来初始化jieba
    cppjieba::Jieba _jieba;   // 用来调用Jieba中的Cut函数
};

// 将sentence的单词切词，并将其存入vector<string>中，返回vector
inline vector<string> SplitToolCppJieba::cut(const string &sentence)
{
    vector<string> words;
    _jieba.Cut(sentence, words, true);
    return words;
}
//判断是否是中文字符
bool isChineseCharacter(char c)
{
    unsigned char uc = static_cast<unsigned char>(c);
    return (uc >= 0xE4 && uc <= 0xE9) || (uc == 0xEA) ||
           (uc >= 0xEB && uc <= 0xED) || (uc == 0xEE) ||
           (uc >= 0xEF && uc <= 0xF0) || (uc >= 0xF1 && uc <= 0xF3) ||
           (uc == 0xF4);
}

// 构建中文的词频文件，将str中的内容分词，并删除停用词、英文等不合法字符
inline map<string, int> SplitToolCppJieba::getWordFrequency(string str)
{
    vector<string> wf;
    map<string, int> mp;
    // 获取停用词
    unordered_set<string> &stopWordList = _conf.getStopWordList();

    // 分词，并存入wf中
    _jieba.Cut(str, wf, true);
    // 遍历wf中的每个词
    for (auto &word : wf)
    {
        // 去除word中的非中文字符
        if (!isChineseCharacter(word[0]))
        {
            word.clear();
            continue;
        }

        auto iter = stopWordList.find(word);
        // 查看单词是否是停用词，是则清空
        if (iter != stopWordList.end())
        {
            word.clear();
            continue;
        }
        // 判断该词是否是英文、数字、标点、空格，是则清空
        for (auto &ch : word)
        {
            if (isalpha(ch) || isdigit(ch) || ispunct(ch) || isspace(ch))
            {
                word.clear();
                continue;
            }
        }
        // 如果单词非空，即符合要求，就存入map，并统计词频
        if (!word.empty())
            ++mp[word];
    }
    return mp;
}

#endif