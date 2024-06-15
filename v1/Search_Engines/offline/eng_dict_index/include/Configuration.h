#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_set>

using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::unordered_set;
using std::ifstream;
using std::istringstream;

class Configuration
{
public:
    // 获取单例，该单例不是线程安全的，不过离线模式不需要多线程，因此简写
    static Configuration& getInstance(const string& filepath)
    {
        static Configuration instance(filepath);
        return instance;
    }
    // Configuration& config = Configuration::getInstance("./");

    // 获取存放在配置文件中文件名和路径的结构体
    map<string, string>& getConfigMap()
    {
        return _configMap;
    }

    // 获取停用词词集
    unordered_set<string>& getStopWordList()
    {
        return _stopWordList;
    }

    // 读取配置文件
    void readConfig()
    {
        ifstream ifs(_filepath);
        if(!ifs)
        {
            cerr << "open file error: " << _filepath  << endl;
            return;
        }

        // 分别是配置文件的一行的内容，内容中的文件名以及对应的路径
        string line, filename, filePath;
        while(!ifs.eof())
        {
            getline(ifs, line);
            istringstream iss(line);
            iss >> filename >> filePath;
            
            // 如果是暂停词文件，我们读取暂停词并放到_stopWordList中
            if(filename.substr(0, 10).compare("stop_words") == 0)
            {
                readStopWord(filePath);
            }
            else
            {
                //如果不是暂停词文件，则放到配置文件中
                _configMap[filename] = filePath;
            }
        }
        ifs.close();
    }

    // 测试函数，可以查看配置文件读取情况
    void print()
    {
        for(auto& elem : _configMap)
        {
            std::cout << elem.first << " " << elem.second << endl;
        }
    }

    ~Configuration() {};
private:
    // 读取暂停词文件
    void readStopWord(const string& filePath)
    {
        ifstream ifs(filePath);
        if(!ifs)
        {
            cerr << "open file error: " << filePath << endl;
            return;
        }

        string stopWord;
        
        // 把暂停词存到_stopWordList中
        while(!ifs.eof())
        {
            ifs >> stopWord;
            _stopWordList.emplace(stopWord);
        }
        ifs.close();
    }
private:
    Configuration(const string& filepath)
    : _filepath(filepath)
    {}

private:
    string _filepath; // 配置文件路径
    map<string, string> _configMap; // 配置文件内容
    unordered_set<string> _stopWordList; // 停用词词集
};



#endif
