#include "../include/MyRedis.hpp"

MyRedis* MyRedis::_pInstance = nullptr;
pthread_once_t MyRedis::_once = PTHREAD_ONCE_INIT;
function<bool()> MyRedis::_initFunc;

// 为了防止每次连接数据库时三次握手，因此我们把redis设置成单例类
// 效果等同于连接池
MyRedis::MyRedis()
:_pConnect(nullptr)
,_pReply(nullptr)
{}

// 释放Redis缓存
MyRedis::~MyRedis()
{
    if(_pReply)
    {
        freeReplyObject(_pReply);
        _pReply = nullptr;
    }

    if(_pConnect)
    {
        redisFree(_pConnect);
        _pConnect = nullptr;
    }
}

MyRedis* MyRedis::getInstance(Configuration* conf)
{
    if(_pInstance == nullptr)
    {
        _initFunc = std::bind(loadConfiguration, conf);
    }
    pthread_once(&_once, init);
    return _pInstance;
}

bool MyRedis::loadConfiguration(Configuration* conf)
{
   map<string, string>& confMap = conf->getConfigMap();

   string ip, port;
   for(auto& elem : confMap)
   {
       if(elem.first == "ip")
       {
           ip = elem.second;
       }
       else if(elem.first == "port")
       {
           port = elem.second;
       }
   }

   cout << "ip: " << ip << " port: " << port << endl;

   bool ret = _pInstance->connect(ip, atoi(port.c_str()));
   return ret;
}

void MyRedis::init()
{
    _pInstance = new MyRedis();
    if(!_initFunc())
    {
        // 如果连接没成功
        destroy();
        return;
    }
    atexit(destroy);
}

void MyRedis::destroy()
{
    if(_pInstance)
    {
        delete _pInstance;
        _pInstance = nullptr;
    }
}

bool MyRedis::connect(const string& host, int port)
{
    _pConnect = redisConnect(host.c_str(), port);
    if(_pConnect == nullptr)
    {
        return false;
    }
    
    if(_pConnect != nullptr && _pConnect->err)
    {
        std::cerr << "connect error: " << _pConnect->errstr << endl;
        return false;
    }

    return true;
}

// 插入键值对
void MyRedis::set(string key, string value)
{
    _pReply = (redisReply*)redisCommand(_pConnect, "SET %s %s", 
                                        key.c_str(), value.c_str());
    if(_pReply)
    {
        freeReplyObject(_pReply);
        _pReply = nullptr;
    }
}

// 返回nullptr说明没有这个数据
string MyRedis::get(string key)
{
    // 发送 GET 命令到 Redis 服务器，并获取键对应的值
    _pReply = (redisReply*)redisCommand(_pConnect, "GET %s",key.c_str());
    if(_pReply)// 检查命令回复
    {
        if(_pReply->type == REDIS_REPLY_STRING)// 检查回复类型是否为字符串
        {
            string str = _pReply->str;// 将回复的字符串值转换为 std::string

            freeReplyObject(_pReply);// 释放回复对象并置为空
            _pReply = nullptr;

            return str;// 返回字符串值
        }
        else
        {
            freeReplyObject(_pReply);// 释放回复对象并返回空字符串
            return string();
        }
    }
    else
    {
        return string();// 命令执行失败，返回空字符串
    }
}

