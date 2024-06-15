#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_set>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::map;
using std::unordered_set;
using std::ifstream;
using std::istringstream;

class Configuration
{
public:
    Configuration(const string& filepath)
    : _filepath(filepath)
    {
        readConfig();
    }

    ~Configuration() {};

    // 存放配置文件
    map<string, string>& getConfigMap()
    {
        return _configMap;
    }

    // 停用词词集
    unordered_set<string>& getStopWordList()
    {
        return _stopWordList;
    }

    //读取配置文件，并把其中的内容存放到数据成员中
    void readConfig()
    {
        ifstream ifs(_filepath);
        if(!ifs)
        {
            cerr << "open file error: " << _filepath  << endl;
            return;
        }
        string line, filename, filePath;
        while(!ifs.eof())
        {
            getline(ifs, line);
            istringstream iss(line);
            iss >> filename >> filePath;
            
            //如果是暂停词文件，我们读取暂停词并放到_stopWordList中
            if(filename.substr(0, 10).compare("stop_words") == 0)
            {
                readStopWord(filePath);
            }
            else
            //如果不是暂停词文件，则放到配置文件中
            {
                _configMap[filename] = filePath;
            }
        }
        ifs.close();
    }

private:
    //如果是暂停词文件，我们读取暂停词并放到_stopWordList中 
    void readStopWord(const string& filePath)
    {
        ifstream ifs(filePath);
        if(!ifs)
        {
            cerr << "open file error: " << filePath << endl;
            return;
        }
        string stopWord;
        while(!ifs.eof())
        {
            ifs >> stopWord;
            _stopWordList.emplace(stopWord);
        }
        ifs.close();
    }

private:
    string _filepath; // 配置文件路径
    map<string, string> _configMap; // 配置文件内容
    unordered_set<string> _stopWordList; // 停用词词集
};

#endif