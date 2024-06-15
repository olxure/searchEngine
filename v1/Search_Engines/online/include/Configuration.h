#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <pthread.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_set>
#include <functional>
#include "NonCopyable.h"

using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::unordered_set;
using std::ifstream;
using std::istringstream;
using std::bind;
using std::function;

// 单例类
class Configuration
:public NonCopyable
{
public:
    static Configuration* getInstance(const string& filepath)
    {
        // 为了防止多次bind
        if(_pInstance == nullptr)
        {
            _initFunc = bind(setFilepath, filepath);
        }
        pthread_once(&_once, init);     //多线程安全

        return _pInstance;
    }

    // 获取配置文件中的文件名和路径
    map<string, string>& getConfigMap()
    {
        return _configMap;
    }

    // 获取停用词词集
    unordered_set<string>& getStopWordList()
    {
        return _stopWordList;
    }

private:
    // 读取配置文件
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

            //如果不是暂停词文件，则放到配置文件中
            _configMap[filename] = filePath;
        }
        ifs.close();
    }

private:
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
    Configuration(const string& filepath)
    : _filepath(filepath)
    {}

    ~Configuration() {};

    static void init(){
        _initFunc();
        atexit(destroy);
    }

    static void setFilepath(const string& filepath)
    {
        _pInstance = new Configuration(filepath);
        _pInstance->readConfig();
    }

    static void destroy()
    {
        if(_pInstance)
        {
            delete _pInstance;
            _pInstance = nullptr;
        }
    }

private:
    static Configuration* _pInstance;
    static pthread_once_t _once;
    static function<void()> _initFunc;       //init内部使用的回调函数

    string _filepath; // 配置文件路径

    map<string, string> _configMap; // 配置文件内容
    unordered_set<string> _stopWordList; // 停用词词集
};


#endif
