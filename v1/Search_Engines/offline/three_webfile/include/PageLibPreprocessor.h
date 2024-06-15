#ifndef __PAGELIBPREPROCESSOR__ONLYONE__
#define __PAGELIBPREPROCESSOR__ONLYONE__
#include "../include/Configuration_jieba.h"
#include "../include/SplitToolCppJieba.h"
#include "../include/cppjieba/Jieba.hpp"
#include "WebPage.h"
#include "PageLib.h"
#include "Configuration.h"
#include "Simhasher.hpp"
#include <string>
#include <map>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <map>
#include <math.h>
#include <string.h>
#define INVERTNAME "../data/invertIndexLib.dat"
#define OFFSETNAME "../data/webOffsetLib.dat"
#define WEBPAGENAME "../data/webPageLib.dat"
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::make_pair;
using std::map;
using std::ofstream;
using std::ostringstream;
using std::pair;
using std::string;
using std::stringstream;
using std::unordered_map;
using std::vector;

struct AllInfo
{
    string content;                                    // 文章的内容
    double all_w;                                      // 文章所有单词的w的平方的总和
    unordered_map<string, pair<int, double>> wordinfo; // 单词+该单词在本文中的数量+该单词在本文中的w;
};
class PageLibPreprocessor
{
public:
    // 构造函数
    PageLibPreprocessor(list<WebPage>& pages, Configuration_jieba &conf);

    // 根据数据结构中的数据生成三个文件，网页库文件、偏移库文件、倒排库文件
    void storeOnDisk();

private:
    // 通过使用中英文词典、网页库数据生成倒排库，并且将内容存储在_invertIndexTable中
    void buildInvertIndexTable();

    // 对冗余的网页进行去重
    void cutRedundantPages();

    //生成倒排索引库文件
    void GetInvertIndexTableFile(const string filename);

    //生成偏移库文件
    void GetOffSetLibFile(const string filename);

    //生成网络库文件
    void GetWebPageLibFile(const string filename);

    //生成倒排索引库的信息，并且将信息存入_offsetLib数据结构中
    void readInfoFormFiles();

    //从v_webpage中提取网页信息生成list<string> _file用于偏移库生成
    void vWebToString();

private:
    /* PageLib &_pageLib;//当程序开始的时候，需要使用此类的函数生成v_webpage中的信息 */
    SplitToolCppJieba _cppJieba;        //将网页库中的文章进行分词时，需要用到此类中的函数
    unordered_map<int, AllInfo> _stringinfo_and_wordinfo; // 文章的id以及文章中所有的信息
    unordered_map<string, unordered_map<int, double>> _invertIndexTable;    // 用于生成对应的倒排索引库

    list<WebPage>& l_webpage;    // 提取的网页库
    vector<WebPage> v_webpage;  // 去重后的网页库

    list<string> _file;         // 用于生成偏移库
    unordered_map<int, pair<int, int>> _offsetLib;      // 存储生成的偏移库
};

#endif
