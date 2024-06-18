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

class MyRedis:public NonCopyable
{
public:
    static MyRedis* getInstance(Configuration* conf);// 获取 MyRedis 类的单例实例。如果实例尚未创建，则创建一个新实例。
    void set(string key, string value);// 设置 Redis 键值对
    string get(string key);// 获取 Redis 键值对

private:
    MyRedis();// 私有构造函数，确保外部无法直接创建对象
    ~MyRedis();

    static void init();// 初始化 MyRedis 单例实例
    static void destroy();// 销毁 MyRedis 单例实例
    static bool loadConfiguration(Configuration* conf);// 加载配置文件中的 Redis 连接信息
    bool connect(const string& host, int port);// 连接 Redis 服务器

private:
    static MyRedis* _pInstance;// 静态成员变量，保存 MyRedis 单例实例的指针
    static pthread_once_t _once;// 静态成员变量，用于确保初始化函数只执行一次
    static function<bool()> _initFunc;// 静态成员变量，保存初始化函数的指针

    redisContext* _pConnect;// Redis 连接上下文
    redisReply* _pReply;// Redis 回复对象
};

