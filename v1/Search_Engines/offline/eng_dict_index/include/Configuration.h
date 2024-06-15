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
    //至于Configuration后面为什么要加引用&考虑到的是单例模式必须保证类实例化出来一个对象的唯一性
    //否则第二次调用getInstance就会又返回一个新的instance对象，这样即便return会销毁一个临时的对象，也还是有多个实例对象
    static Configuration& getInstance(const string& filepath)//接收和使用常量字符串，同时const加引用减少拷贝开销，
    //这是静态成员函数，不是构造函数，它通过返回对 static Configuration instance 的引用来实现单例模式。这是提供访问单例实例的全局访问点。
    {
        //在C++中，static局部变量的初始化只会在它第一次被执行时进行，并且只会进行一次
        //因此，在第二次调用getInstance时，static Singleton instance;不会再次创建一个新的实例，
        //而是直接返回已经创建的那个实例。这是C++中静态局部变量的一个关键特性
        //静态局部变量：在函数内部声明的静态局部变量只在函数第一次被调用时初始化，并在函数退出后仍然存在。每次调用该函数时都会返回同一个变量
        static Configuration instance(filepath);//静态局部变量只会在首次调用初始化
        //这才是构造函数，相当于前向声明，在private中给实例化的对象instance中的_filepath初始化filepath，这样对象不会像变量一样没有赋值
        return instance;//static定义的是这个instance，说明它是一个局部变量
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
        //创建输入文件流对象并打开名为"filename.txt"的文件
        //这个_filepath就是构造函数那个单例的对象的private的那个被初始化列表的_filepath
        ifstream ifs(_filepath);
        if(!ifs)
        {
            cerr << "open file error: " << _filepath  << endl;
            return;
        }

        // 分别是配置文件的一行的内容，内容中的文件名以及对应的路径
        string line, filename, filePath;//重新定义string类对象，数据成员还没有初始化
        while(!ifs.eof())//打开的文件中如果读到末尾，循环结束
        {
            //std::getline 函数的返回值是 std::istream&，即输入流的引用。这意味着可以通过检查返回值来确定读取操作是否成功
            getline(ifs, line);//一行一行输，ifs->line,循环一次读一行，可以正常读空行
            istringstream iss(line);//字符串输入流，类比文件输入流，
            //从 std::istringstream 对象 iss 中提取数据，并将其存储到变量 filename 和 filePath 中。
            iss >> filename >> filePath;
            //cerr << filename << " "<< filePath << "\n";//测试行打印用,eng.conf文件中不能通过注释来单个文件测试，要么删掉配置文件路径
            //否则会出现# english.txt这样的分割

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
        ifs.close();//显式关闭
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
