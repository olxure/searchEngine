#include "../include/Configuration.h"
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <ctype.h>

using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::map;
using std::set;
using std::vector;
using std::ofstream;

// 生成英文词典以及英文索引库
class EngDictProducer
{
public:
    EngDictProducer(Configuration& conf)
    :_conf(conf)
    {
        readFilePath();
    }

    void storeDict(const string& filename);        //将词典库写入到文件中
    void storeIndex(const string& filename);    //将索引库写入到文件
    void createDict();      //创建词典库

private:
    void readFilePath();    //读取文件路劲
    void addFileWord(const string& filename);     //添加文章单词
    string dealWord(const string& word);    //处理单词
    void addIndex(const string & word, int lineNumber); //添加索引
    void dealLine(string& line);        //处理一行

private:
    Configuration& _conf;       //配置文件
    vector<string> _files;      //语料库文件路径集合
    map<string, int> _dict;     //英文词典库
    map<string, set<int>> _index; //英文索引库
};

void EngDictProducer::readFilePath()
{
    map<string, string>& _configMap = _conf.getConfigMap();
    for(auto& elem : _configMap)
    {
        _files.push_back(elem.second);  //读取所有路劲文件
    }
}

void EngDictProducer::createDict()
{
    for(auto& filePath : _files)
    {
        addFileWord(filePath);  //把每一个语料库文件添加到_dict中
    }
}

string EngDictProducer::dealWord(const string& word)
{
    unordered_set<string>& stopWordList = _conf.getStopWordList();

    string result;
    for(auto& elem : word)
    {
        if(isalpha(elem))       //若是字母
        {
            char c = tolower(elem); //转化成小写
            result.push_back(c);
        }
    }

    auto iter = stopWordList.find(result);  //查看一下处理完的单词是否是暂停词
    if(iter != stopWordList.end())
    {
        result.clear();
    }

    return result;
}

void EngDictProducer::dealLine(string& line)
{
    for(auto& elem : line)
    {
        if(ispunct(elem))           //删除标点符号
        {
            elem = ' ';
        }
    }
}

void EngDictProducer::addFileWord(const string& filename)
{

    ifstream ifs(filename);
    if(!ifs)
    {
        cerr << "open file error: " << filename << endl;
        return;
    }

    string line, word;
    while(!ifs.eof())
    {
        getline(ifs, line);
        dealLine(line);    // 先把一行里面的标点符号全部变成空格

        istringstream iss(line);

        while(!iss.eof())
        {
            iss >> word;
            string result = dealWord(word); // 再把一行里面的不是字母的去除，且全部转化成小写
            if(result != string())
            {
                ++_dict[result];    // 将处理完的单词添加到词典库
            }
        }
    }
    ifs.close();
}

// 生成词典库文件和索引库表
void EngDictProducer::storeDict(const string& filename)
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
        // 在把内容存到词典库文件的同时把行号记录到索引表中
        ofs << elem.first << " " << elem.second << endl;
        addIndex(elem.first, lineNumber);
        ++lineNumber;
    }

    ofs.close();
}

// 把单词的每个字母的行号记录到索引表
void EngDictProducer::addIndex(const string& word, int lineNumber)
{
    char buf[2] = {0};
    for(auto& elem : word)
    {
        buf[0] = elem;
        _index[string(buf)].insert(lineNumber);
    }
}

// 生成索引表文件
void EngDictProducer::storeIndex(const string& filename)
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

int main(int argc, char* argv[])
{
    Configuration& conf = Configuration::getInstance(argv[1]);

    conf.readConfig();      // 读取配置文件

    EngDictProducer dict(conf);

    dict.createDict();      // 生成词典库
    dict.storeDict("eng_dict.txt");     //存储词典和生成索引库
    dict.storeIndex("eng_index.txt");   //存储词典库
    return 0;
}

