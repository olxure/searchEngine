#pragma once
#include <cstring>
#include "ThreadPool.h"
#include "TcpServer.h"
#include "TcpConnection.h"
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

class MyTask
{
public:
    MyTask(const char* msg, const TcpConnectionPtr &con, CacheManager* cacheManager, pthread_rwlock_t& rwlock)
    : _msg(msg)
    , _con(con)
    , _cacheManager(cacheManager)
    , _rwlock(rwlock)
    {}

    //运行在某一个线程中
    void process(string confPath)
    {
        Configuration* conf = Configuration::getInstance(confPath);
        
        int messageLength = 0;
        int protocol = 0;

        string msg;     // 发送

        //decode
        messageLength = *(int*)(_msg);  //收到用户名的长度
        protocol = *(int*)(_msg + 4);   //协议类型

        cout << "messageLength: " << messageLength << " protocol: " << protocol << endl;


        string usrMsg(_msg+8);          // 用户发送的内容
        cout << "usrMsg: " << usrMsg << endl;

        if (protocol == 1) {
            // 关键词推荐
            int idx = atoi(current_thread::name);
            LRUCache& cacheA = _cacheManager->GetCache(idx);
            LRUCache& cacheB = _cacheManager->GetSapreCache(idx);

            cout << "cache" << endl;
            pthread_rwlock_rdlock(&_rwlock);
            if(0 == update_flag)
            {
                // 先去缓存中查找
                cacheA.print();
                json result = cacheA.getElementWithLRU(usrMsg);
                cacheB.getElementWithLRU(usrMsg);
                cout << "jsonResult: " << result << endl;
                cacheA.print();
                
                // 若缓存没有命中
                if(result.empty())
                {
                    result = findInLib(usrMsg, conf);
                    
                    cacheA.addElement(usrMsg, result);
                    cacheB.addElement(usrMsg, result);
                }
                pthread_rwlock_unlock(&_rwlock);   //解除读锁

                // 把结果包装小火车
                generateMsg(msg, result);            
            }
            // A更新B服务（只读）
            else if(1 == update_flag)
            {
                json result = cacheB.getElementWithoutLRU(usrMsg);
                pthread_rwlock_unlock(&_rwlock);        // 解除读锁

                if(result.empty())
                {
                    result = findInLib(usrMsg, conf);
                }
                generateMsg(msg, result);
            }
            // B更新A服务（只读）
            else if(2 == update_flag)
            {
                json result = cacheA.getElementWithoutLRU(usrMsg);
                pthread_rwlock_unlock(&_rwlock);        // 解除读锁

                if(result.empty())
                {
                    result = findInLib(usrMsg, conf);
                }

                generateMsg(msg, result);
            }

        }
        else if (protocol == 2) {
            // 网页搜索
            string name = usrMsg.substr(0, messageLength);
            string realMsg = usrMsg.substr(messageLength);
            json result = webSearch(conf, name, realMsg);
            generateMsg(msg, result);
        }
        else if(protocol == 3)
        {
            // 登入操作
            string name = usrMsg.substr(0, messageLength);
            string passwd = usrMsg.substr(messageLength);
            int ret = login(name, passwd);
            generateMsg(msg, ret);
        }
        else if(protocol == 4)
        {
            // 注册操作
            string name = usrMsg.substr(0, messageLength);
            string passwd = usrMsg.substr(messageLength);
            signUp(name, passwd);
        }
        else if(protocol == 5)
        {
            // 历史记录
            json result = json::parse(getUserHistoryJSON(usrMsg));        
            cout << "result" << result << endl;
            generateMsg(msg, result);
        }

        /*
        //后面项目的时候，将业务逻辑在此处添加
        //string msg = _con->receive();//拿到客户端发送过来的数据

        }*/

        //compute
        //encode  ==> 封装一个JSON对象 
        //方案一: 
        // 通过pthread_t找到一个缓存
        // 问题在于缓存需要依赖于线程库 map<pthread_t, LRUCache> caches;
        //pthread_t pid = pthread_self();
        //
        //方案二: 使用线程局部存储
        //1号线程 对应与下标 1
        cout << "current_thread::name:" << current_thread::name << endl;
        //int idx = atoi(current_thread::name);
        //LRUCache & cache = CacheManager::getCache(idx);
        if(protocol != 4)
        {
            _con->sendInLoop(msg);
        }

        delete[] _msg;  // 释放小火车
    }
    
    void process2()
    {
        string msg;
        json result = json::parse(getTopTenHistoryJSON());
        cout << "result: " << result << endl;
        generateMsg(msg, result);

        _con->sendInLoop(msg);
    }

    void processTimeout()
    {
        cout << "current_thread::name:" << current_thread::name << " do timeout task!" << endl;
        LogInfo("LRUCache has updated! write to file.");
        _cacheManager->UpdateCache();
    }
private:
    json webSearch(Configuration* conf, string& name, string& usrMsg);
    string getUserHistoryJSON(const string& name);
    void pushHistory(const string& usrname, const string& str);

    string getTopTenHistoryJSON();
    int login(string name, string passwd);

    void signUp(string name, string password);
    char* genRandomString(int length);
    void getNamePasswd(char* name, char* passwd);
    
    void generateMsg(string& msg, json& j)
    {
        int messageLength = j.dump().size();
        msg.append(reinterpret_cast<char*>(&messageLength), sizeof(messageLength));
        msg.append(j.dump());
    }   

    void generateMsg(string& msg, int data)
    {
        int messageLength = sizeof(data);
        msg.append(reinterpret_cast<char*>(&messageLength), sizeof(messageLength));
        msg.append(reinterpret_cast<char*>(&data), sizeof(data));
        cout << "data: " << *(int*)(msg.data()+4) << endl;
    }   
    
    json findInLib(string& usrMsg, Configuration* conf)
    {
        Dictionary* d1 = Dictionary::getInstance(conf);
        KeyRecommander keyTest(usrMsg, *d1);
        
        keyTest.statistic();
        
        vector<string> strings = keyTest.getResult();
        
        json jsonResult;
        cout << "findInLib: ";
        for(auto it = strings.begin(); it != strings.end(); ++it)
        {
            cout << *it << " ";
            jsonResult.push_back(*it);
        }
        cout << endl;
        return jsonResult;
    }

private:
    const char* _msg;
    TcpConnectionPtr _con;
    CacheManager* _cacheManager;
    pthread_rwlock_t& _rwlock;
};
