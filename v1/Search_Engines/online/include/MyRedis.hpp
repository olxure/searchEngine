#pragma once

#include "Configuration.h"
#include "NonCopyable.h"
#include <hiredis/hiredis.h>
#include <pthread.h>
#include <iostream>
#include <functional>

using std::cout;
using std::endl;
using std::string;

class MyRedis
:public NonCopyable
{
public:
    static MyRedis* getInstance(Configuration* conf);
    void set(string key, string value);
    string get(string key);

private:
    MyRedis();
    ~MyRedis();

    static void init();
    static void destroy();
    static bool loadConfiguration(Configuration* conf);
    bool connect(const string& host, int port);

private:
    static MyRedis* _pInstance;
    static pthread_once_t _once;
    static function<bool()> _initFunc;

    redisContext* _pConnect;
    redisReply* _pReply;
};

