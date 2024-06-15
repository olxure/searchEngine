#ifndef __CACHEMANAGER__ONLYONE__
#define __CACHEMANAGER__ONLYONE__
#include "LRUCache.h"
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>
#include <map>
#include <pthread.h>
using std::ostringstream;
using json = nlohmann::json;
using std::cout;
using std::endl;
using std::make_pair;
using std::map;
using std::string;
using std::vector;
//用来获取更新状态的数据结构
extern int update_flag;

class CacheManager
{
public:
    //用来生成单例类的函数
    static CacheManager *GetCacheManager();
    //用来销毁单例类的函数
    static void DestroyCacheManager();
    //拉起8个LRU缓存的函数
    void init(string filename);
    //提供的接口，方便其他调用者获取对应的LRU对象
    LRUCache &GetCache(int thread_num);
    //提供的接口，方便其他调用者在更新时获取对应的LRU对象
    LRUCache &GetSapreCache(int thread_num);
    //更新所有的URL对象，更新Cache中的数据
    void UpdateCache();
    //获取读写锁——接口
    pthread_rwlock_t& getRWlock();

private:
//构造
    CacheManager();
//析构
    ~CacheManager();
    //获取所有LRU中pending result list中的查询记录个数
    int GetAllPendingUpdateListNum();
    //为了公平起见，将四个LRUCache中的pendingresultlist中的更新数据进行计算，获取相同比例的数据，当数量大于一千时使其总数加起来等于一千
    void GetUsefulNum(vector<int> &);
    //更新第一个LRUCache的数据
    void UpdateFirstCache(list<pair<string, json>> &update_list);
    //使用第一个LRUCache中的数据更新其他三个LRU对象
    void UpdateAllCacheByFirst();

private: 
//用来存储本对象的地址，需要再cpp实现文件中进行初始化
    static CacheManager *p_CacheManager;
    //存储的是正式的LRU对象
    vector<LRUCache> v_caches;
    //存储的是当更新时提供服务的LRU对象
    vector<LRUCache> v_spare_caches;
    /* pthread_mutex_t _mutex_lock; */
    //读写锁的数据结构
    pthread_rwlock_t _rwlock;
};

#endif
