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

    void initDictCh(const string &dictPathCh);// 初始化中文词典
    void initDictEn(const string &dictPathEn);// 初始化英文词典

    void initIndexCh(const string &indexPathCh);// 初始化中文索引
    void initIndexEn(const string &indexPathEn);// 初始化英文索引

    vector<pair<string,int>>& getDictCh();// 获取中文词典,返回对应的库
    vector<pair<string,int>>& getDictEn();// 获取英文词典

    map<string,set<int>>& getIndexCh();// 获取中文索引
    map<string,set<int>>& getIndexEn();// 获取英文索引
    //vector<pair<string,int>>& getFinRes();

    set<int> findWord(const string& ch,map<string,set<int>> _index);// 查找词语出现的行号，返回找到string的容器

    vector<pair<string,int>>& doAndQuery(const string & str);//// 进行与查询，使用分词库和词频库查找字符串中 包含字符的单词与频率
    
    bool isChinese(const string& ch);// 判断是否为中文
    bool isEnglish(const string& ch);// 判断是否为英文
    void CutWord(const string words, vector<string> &wordset);// 切分词语

private:
    vector<pair<string,int>> _dictCh;

    map<string,set<int>> _indexCh;
    
    vector<pair<string,int>> _dictEn;
    
    map<string,set<int>> _indexEn;
    
    vector<pair<string,int>> _RecWord;
    
    static Dictionary* m_dictionary;// 单例指针
    static std::mutex m_Mutex;// 互斥锁
};

