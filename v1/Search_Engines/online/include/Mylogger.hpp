#ifndef __MYLOG__
#define __MYLOG__
#include <iostream>
#include <log4cpp/Category.hh>
#include <pthread.h>
#include "NonCopyable.h"

using std::cout;
using std::endl;
using std::cin;
using std::string;

class Mylogger
:public NonCopyable
{
public:
    static Mylogger* getInstance()
    {
        pthread_once(&_once, init);
        return _pInstance;
    }
    
    template<class ...Args>
    void info(const char* msg, Args...agrs);
    
    template<class ...Args>
    void error(const char* msg, Args...args);
    
    template<class ...Args>
    void warn(const char* msg, Args...args);

    template<class ...Args>
    void debug(const char* msg, Args...args);
private:
    static void init()
    {
        _pInstance = new Mylogger();
        atexit(destroy);
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
    Mylogger(); 
    ~Mylogger();
    log4cpp::Category& _mylog;
    static Mylogger* _pInstance;
    static pthread_once_t _once;
};

#define SETMSG(msg)   \
    string("[").append(__FILE__)    \
    .append(":").append(__func__)   \
    .append(":").append(std::to_string(__LINE__))   \
    .append("] ").append(msg).c_str()   


#define LogInfo(msg, ...) Mylogger::getInstance()->info(SETMSG(msg), ##__VA_ARGS__)
#define LogError(msg, ...) Mylogger::getInstance()->error(SETMSG(msg), ##__VA_ARGS__)
#define LogWarn(msg, ...) Mylogger::getInstance()->warn(SETMSG(msg), ##__VA_ARGS__)
#define LogDebug(msg, ...) Mylogger::getInstance()->debug(SETMSG(msg), ##__VA_ARGS__)


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
