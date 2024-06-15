#ifndef WD_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_CONFIGURATION_H_
#define WD_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_CONFIGURATION_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>

using std::map;
using std::set;
using std::string;

class Configuration {
public:
    // Get the singleton instance (lazy initialization)
    static Configuration& getInstance(const string& filepath) {
        static Configuration instance(filepath);//真正的构造函数，不过是前向声明，实现在后面，初始化private的数据成员
        return instance;
    }

    // 获取配置映射，返回值类型是一个指向 map<string, string> 的引用
    map<string, string>& getConfigMap() {
        return _configMap;
    }

    // Get the stop word list
    set<string>& getStopWordList() {
        return _stopWordList;
    }

private:
    //构造函数放在private中，保证不被其他的函数随时构造
    Configuration(const string& filepath)
        : _filepath(filepath) {
        std::ifstream configFile(_filepath);//相当于ifs输入文件流，并打开../conf/config.txt文件
        if (configFile.is_open()) {// 检查文件是否成功打开，成功则继续执行if中的操作
            string line;//重新定义新的类line，未初始化
            while (std::getline(configFile, line)) {//
            //std::getline 函数用于从输入流中读取一行文本，并将其存储在指定的字符串变量中。它的返回值是输入流对象本身，即std::istream&
            //可以用于检查读取操作是否成功。如果读取成功，返回值可以转换为 true；如果读取失败（例如，到达文件末尾或发生错误），返回值可以转换为 false。
            //即configFile中的一行流向->line，成功了会自动转化为true
                std::istringstream iss(line);//字符串输入流
                //初始化 iss 时，line 中的内容被复制到 iss 对象中。这样你就可以像操作输入流（例如文件流或标准输入流）那样来操作这个字符串。
                string key, value;//用来接收打开的../conf/config.txt文件中的键值对，要保证配置文件以空格分开
                if (iss >> key >> value) {
                    _configMap[key] = value;
                }
            }

            configFile.close();//显式关闭
        } else {
            std::cout << "Failed to open config file." << std::endl;
            throw std::runtime_error("Error processing configuration file");
        }
    }

    ~Configuration() {}//单例模式析构函数也放在private中

private:
    string _filepath;               // ../conf/config.txt
    map<string, string> _configMap;  // 用于存储配置文件中的键值对，即配置文件具体内容，构造函数中会被初始化
    set<string> _stopWordList;       // Stop word list
};

#endif

