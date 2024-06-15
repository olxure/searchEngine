#ifndef __CHIDICTPRODUCER_H__
#define __CHIDICTPRODUCER_H__

#include "../include/Configuration.h"
#include "../include/SplitToolCppJieba.h"
#include <set>
#include <ctype.h>
#include <regex>

using std::cin;
using std::set;
using std::ofstream;
using std::stringstream;

class ChiDictProducer
{
public:
    ChiDictProducer(Configuration &conf, SplitToolCppJieba& Jieba)
    :_conf(conf)
    ,_jieba(Jieba)
    {
        readFilePath();
    }

    void storeDict(const string& filename);     //将词典库写入到文件中
    void storeIndex(const string& filename);    //将索引库写入到文件
    void createDict();                          //创建词典库
    void CutWord(const string words, vector<string> &wordset);  //切割中文词为字

private:
    void readFilePath();                        //读取文件路径
    void addFileWord(const string& filename);   //把filename的语料库文件的单词加入词典
    void addIndex(const string & word, int lineNumber); //添加索引

private:
    Configuration& _conf;           //配置文件
    vector<string> _files;          //语料库文件绝对路径集合
    map<string, int> _dict;         //中文词典库
    map<string, set<int>> _index;   //中文索引库
    SplitToolCppJieba &_jieba;      //分词工具
};

//将语料库的每个文件的路径存入_files
void ChiDictProducer::readFilePath()
{
    map<string, string>& _configMap = _conf.getConfigMap();
    for(auto& elem : _configMap)
    {
        _files.push_back(elem.second);  //读取所有绝对路径文件
    }
}

//创建词典，统计词频（读入每一个语料库文件）
void ChiDictProducer::createDict()
{
    for(auto& filePath : _files)
    {
        addFileWord(filePath);  //把每一个语料库文件添加到_dict中
    }
}

//把filename文件中的单词统计词频放入词典
void ChiDictProducer::addFileWord(const string& filename)
{
    ifstream ifs(filename);
    if(!ifs)
    {
        cerr << "open file error: " << filename << endl;
        return;
    }

    //一次性把文件中的内容全部读入到str中
    stringstream buf;
    buf << ifs.rdbuf();
    string str(buf.str());

    //处理该文件，统计该文件中的词频并返回
    map<string, int> temp = _jieba.getWordFrequency(str);
    //遍历该文件的词频，合并到词典中
    for(auto &ch : temp)
    {
        _dict[ch.first] += ch.second;
    }
    ifs.close();
}

//识别词语中的汉字，并把每个字存入wordset中
void ChiDictProducer::CutWord(const string words, vector<string> &wordset)
{
  for (size_t i = 0; i < words.size();)
  {
    int cplen = 1;
    if ((words[i] & 0xf8) == 0xf0)
      cplen = 4;
    else if ((words[i] & 0xf0) == 0xe0)
      cplen = 3;
    else if ((words[i] & 0xe0) == 0xc0)
      cplen = 2;
    if ((i + cplen) > words.length())
      cplen = 1;
    wordset.push_back(words.substr(i, cplen));
    i += cplen;
  }
  return ;
}

//分解每个词，并统计这些个汉字的行号，存入数据成员_index中
void ChiDictProducer::addIndex(const string& word, int lineNumber)
{
    vector<string> wordsets;
    //将每个中文词语分解为汉字，存储在wordsets中
    CutWord(word, wordsets);
    for(auto& elem : wordsets)
    { 
        //插入每个汉字出现在字典中的行数
        _index[(string)elem].insert(lineNumber);
    }
    wordsets.clear();
}

//将词典存入文件，永久保存
void ChiDictProducer::storeDict(const string& filename)
{
    ofstream ofs(filename);
    if(!ofs)
    {
        cerr << "open file error: " << filename << endl;
        return;
    }

    int lineNumber = 1;
    for(auto& elem : _dict)
    {
        ofs << elem.first << " " << elem.second << endl;
        //分解每个词，并统计这些个汉字的行号
        addIndex(elem.first, lineNumber);
        ++lineNumber;
    }

    ofs.close();
}

//将索引存入文件，永久保存
void ChiDictProducer::storeIndex(const string& filename)
{
    ofstream ofs(filename);
    if(!ofs)
    {
        cerr << "open file error: " << filename << endl;
        return;
    }
    
    for(auto& elem : _index)
    {
        ofs << elem.first;
        
        for(auto& number : elem.second)
        {
            ofs << " " << number;
        }
        ofs << endl;
    }
    ofs.close();
}

#endif