#ifndef __ECHOSERVER_H__
#include<cstring>
#include "ThreadPool.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "MyTask.h"
#include "../../include/Mylogger.hpp"
#include "../../include/Dictionary.h"
#include "../../include/KeyRecommander.h"
#include "../../include/WebPageQuery.hpp"
#include "../../include/MyRedis.hpp"
#include "../../include/include/WebPageSearcher.h"
#include "../../include/CacheManager.hh"
#include "../../include/LRUCache.h"

using json=nlohmann::json;
using std::cout;
using std::endl;

class EchoServer
{
public:
    EchoServer(size_t threadNum, size_t queSize
               , const string &ip
               , unsigned short port
               , string confPath)
    : _pool(threadNum, queSize)
    , _server(ip, port)
    , _cacheManager(CacheManager::GetCacheManager())
    , _confPath(confPath)
    {}

    ~EchoServer()
    {}

    void start()
    {
        _pool.start();// 启动线程池
        Configuration* conf = Configuration::getInstance(_confPath);// 获取配置实例
        map<string, string> configMap = conf->getConfigMap();// 获取配置文件中的配置信息
        for(auto& elem : configMap)// 初始化缓存管理器
        {
            if(elem.first == "cache_data.dat")
            {
                _cacheManager->init(elem.second);
            }
        }
        /* _server.setAllCallback(std::move(onNewConnection) */
        /*                        , std::move(onMessage) */
        /*                        , std::move(onClose)); */
        //// 使用 std::bind 绑定回调函数到服务器
        using namespace std::placeholders;
        _server.setAllCallback(std::bind(&EchoServer::onNewConnection, this, _1)
                               , std::bind(&EchoServer::onMessage, this, _1, _confPath)
                               , std::bind(&EchoServer::onClose, this, _1)
                               , std::bind(&EchoServer::onTimeout, this, _1));
        _server.start();// 启动服务器
    }
    void stop()
    {
        _pool.stop();
        _server.stop();
    }


private:
    void onTimeout(const TcpConnectionPtr &con)
    {
        cout << endl << "clock is timeout! begin update." << endl;
        char* msg = nullptr;
        MyTask task(msg, con, _cacheManager, _cacheManager->getRWlock());

        _pool.addTask(std::bind(&MyTask::processTimeout, task));
    }
    //1、连接建立做的事件
    void onNewConnection(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has connected!" << endl;
        // 验证账户是否存在
        // TODO...
        /* char* name = "user"; */
        /* LogInfo("Info: user = %s has connected!", name); */
        char* msg;
        MyTask task(msg, con, _cacheManager, _cacheManager->getRWlock());
        _pool.addTask(std::bind(&MyTask::process2, task));
    }
    
    //2、消息到达做的事件
    void onMessage(const TcpConnectionPtr &con, string confPath)
    {
        // 记得使用堆空间
        char* msg = con->receive();//接收客户端的数据
        cout << ">>recv msg from client " << endl;
    
        //接收到客户端的msg之后，是可以进行业务逻辑的处理
        MyTask task(msg, con, _cacheManager, _cacheManager->getRWlock());
        //添加了Task，添加到了线程池里面去了,执行了_taskQue
        //中的push
        _pool.addTask(std::bind(&MyTask::process, task, confPath));
    }
    
    //3、连接断开做的事件
    void onClose(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has closed!" << endl;
    }

private:
    ThreadPool _pool;
    TcpServer _server;
    CacheManager* _cacheManager;
    string _confPath;
};

#endif
