#pragma once
#include <iostream>
#include <string>
#include <mutex>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>  // 包含 make_pair 函数
#include "Configuration.h"

using namespace std;

class Dictionary//字典单例类
{
private:
    Dictionary(){}//单例类将构造析构拷贝构造赋值运算符设置为私有模式
    ~Dictionary(){}
    Dictionary(const Dictionary&)=delete;
    Dictionary& operator=(const Dictionary&)=delete;


public:
    static Dictionary *getInstance(Configuration* conf);//获取单例类
    static void deleteInstance();//删除单例类

    void initDictCh(const string &dictPathCh);//获取字典库
    void initDictEn(const string &dictPathEn);

    void initIndexCh(const string &indexPathCh);
    void initIndexEn(const string &indexPathEn);

    vector<pair<string,int>>& getDictCh();//返回对应的库
    vector<pair<string,int>>& getDictEn();

    map<string,set<int>>& getIndexCh();
    map<string,set<int>>& getIndexEn();
    //vector<pair<string,int>>& getFinRes();

    set<int> findWord(const string& ch,map<string,set<int>> _index);//返回找到string的容器

    vector<pair<string,int>>& doAndQuery(const string & str);//使用分词库和词频库查找字符串中 包含字符的单词与频率
    
    bool isChinese(const string& ch);//判断中英文字符
    bool isEnglish(const string& ch);
    void CutWord(const string words, vector<string> &wordset);

private:
    vector<pair<string,int>> _dictCh;

    map<string,set<int>> _indexCh;
    
    vector<pair<string,int>> _dictEn;
    
    map<string,set<int>> _indexEn;
    
    vector<pair<string,int>> _RecWord;
    
    static Dictionary* m_dictionary;
    static std::mutex m_Mutex;
};

