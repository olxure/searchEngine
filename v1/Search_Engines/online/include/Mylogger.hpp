#ifndef __MYLOG__
#define __MYLOG__
#include <iostream>
#include <log4cpp/Category.hh>
#include <pthread.h>
#include "NonCopyable.h"// 自定义的不可复制类，防止复制

using std::cout;
using std::endl;
using std::cin;
using std::string;

// Mylogger类，继承自NonCopyable，确保该类对象不可复制
class Mylogger:public NonCopyable
{
public:
    // 获取单例实例
    static Mylogger* getInstance()
    {
        pthread_once(&_once, init);// 确保init函数只被调用一次
        return _pInstance;
    }
    
    // 日志记录方法
    template<class ...Args>
    void info(const char* msg, Args...agrs);
    
    template<class ...Args>
    void error(const char* msg, Args...args);
    
    template<class ...Args>
    void warn(const char* msg, Args...args);

    template<class ...Args>
    void debug(const char* msg, Args...args);
private:
    // 初始化单例实例
    static void init()
    {
        _pInstance = new Mylogger();// 创建单例实例
        atexit(destroy);// 程序退出时销毁单例实例
    }

    // 销毁单例实例
    static void destroy()
    {
        if(_pInstance)
        {
            delete _pInstance;// 释放单例实例
            _pInstance = nullptr;
        }
    }
private:
    // 私有构造函数和析构函数，防止外部创建和销毁对象
    Mylogger(); 
    ~Mylogger();
    log4cpp::Category& _mylog;// 日志类别
    static Mylogger* _pInstance;// 单例实例指针
    static pthread_once_t _once;// 用于确保init函数只被调用一次
};

// 辅助宏，用于在日志信息中添加文件名、函数名和行号
#define SETMSG(msg)   \
    string("[").append(__FILE__)    \
    .append(":").append(__func__)   \
    .append(":").append(std::to_string(__LINE__))   \
    .append("] ").append(msg).c_str()   

// 定义宏，用于简化日志记录函数的调用
#define LogInfo(msg, ...) Mylogger::getInstance()->info(SETMSG(msg), ##__VA_ARGS__)
#define LogError(msg, ...) Mylogger::getInstance()->error(SETMSG(msg), ##__VA_ARGS__)
#define LogWarn(msg, ...) Mylogger::getInstance()->warn(SETMSG(msg), ##__VA_ARGS__)
#define LogDebug(msg, ...) Mylogger::getInstance()->debug(SETMSG(msg), ##__VA_ARGS__)

// 模板函数实现，用于记录不同级别的日志
template<class ...Args>
void Mylogger::warn(const char* msg, Args...args)
{
    _mylog.warn(msg, args...);
}

template<class ...Args>
void Mylogger::error(const char* msg, Args...args)
{
    _mylog.error(msg, args...);
}

template<class ...Args>
void Mylogger::info(const char* msg, Args...args)
{
    _mylog.info(msg, args...);
}

template<class ...Args>
void Mylogger::debug(const char* msg, Args...args)
{
    _mylog.info(msg, args...);
}

#endif
