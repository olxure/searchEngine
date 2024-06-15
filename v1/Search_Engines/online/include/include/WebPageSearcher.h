#ifndef __WEBPAGESEARCHER_H__
#define __WEBPAGESEARCHER_H__

#include <vector>
#include <string>
#include <iostream>
#include "./SplitToolCppJieba.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

class WebPageSearcher
{
public:
    WebPageSearcher(const string str, SplitToolCppJieba &jieba)
        : _queryLine(str), _jieba(jieba)
    {
        cutWordbyJieba();
    }

    // 直接用jieba分词
    void cutWordbyJieba()
    {
        // 调用cppjieba处理中英文分词
        vector<string> temp = _jieba.cut(_queryLine);
        for (auto &word : temp)
        {
            _wordList.push_back(word);
        }
    }

    vector<string> &getWordList()
    {
        return _wordList;
    }

private:
    string _queryLine;
    vector<string> _wordList;
    SplitToolCppJieba &_jieba; // 分词工具
};

#endif