#include "../include/CacheManager.hh"
// 更新标志位的初始化
int update_flag;
// 该类对象的地址的存储位置的初始化
CacheManager *CacheManager::p_CacheManager = nullptr;
// 获取单例对象的指针
CacheManager *CacheManager::GetCacheManager()
{
    if (p_CacheManager == nullptr)
    {
        p_CacheManager = new CacheManager();
    }
    return p_CacheManager;
}
// 销毁单例对象
void CacheManager::DestroyCacheManager()
{
    if (p_CacheManager != nullptr)
    {
        delete p_CacheManager;
        p_CacheManager = nullptr;
    }
}

void CacheManager::init(string filename)
{ // 初始化，创建四个LRUCache对象并且将其存放在vector中
    for (int i = 0; i < 4; i++)
    {
        LRUCache temp(filename);
        v_caches.emplace_back(temp);
    }
    // 创建四个备胎LRUCache对象并且将其放在备用的vector中
    for (int i = 0; i < 4; i++)
    {
        LRUCache temp(filename);
        v_spare_caches.emplace_back(temp);
    }
}
//返回对应小标的LRUCache对象
LRUCache &CacheManager::GetCache(int thread_num)
{
    return v_caches[thread_num];
}
//返回对应下标的LRUCache的备胎对象
LRUCache &CacheManager::GetSapreCache(int thread_num)
{
    return v_spare_caches[thread_num];
}

//更新八个LRU对象中的数据
void CacheManager::UpdateCache()
{
    //更新开始
    cout << "UpdateCache begin!" << endl;
    // 将标志位设置为1，告诉lru对象开始更新a数据，让a关闭main的读写服务，打开b的读服务
    /* pthread_mutex_lock(&_mutex_lock); */
    pthread_rwlock_wrlock(&_rwlock);
    update_flag = 1;
    pthread_rwlock_unlock(&_rwlock);
    /* pthread_mutex_unlock(&_mutex_lock); */
    list<pair<string, json>> all_update_info;

    if (GetAllPendingUpdateListNum() > 1000)
    {

        vector<int> each_num;
        GetUsefulNum(each_num);
        for (int i = 0; i < 4; i++)
        {
            list<pair<string, json>> &temp = v_caches[i].getPendingUpdateList();
            auto it = temp.begin();
            for (int w = 0; w < each_num[i]; w++)
            {
                it++;
            }
            all_update_info.splice(all_update_info.begin(), temp, temp.begin(), it);
            temp.clear();
        }
    }
    else
    {
        // 如果小于1000直接获取所有的缓存
        list<pair<string, json>> &temp = v_caches[0].getPendingUpdateList();
        temp.clear();
        for (int i = 1; i < 4; i++)
        {
            list<pair<string, json>> &temp1 = v_caches[i].getPendingUpdateList();
            all_update_info.splice(all_update_info.begin(), temp1);
        }
    }
    // 将待更新数据写入第一缓存
    UpdateFirstCache(all_update_info);
    UpdateAllCacheByFirst();
    // 将标志位设置为2，告诉lru对象开始更新b数据，打开a的读服务关闭a的写服务，关闭b的读服务
    pthread_rwlock_wrlock(&_rwlock);
    update_flag = 2;
    pthread_rwlock_unlock(&_rwlock);
    /* pthread_mutex_lock(&_mutex_lock); */
    /* update_flag = 2; */
    /* pthread_mutex_unlock(&_mutex_lock); */
    list<pair<string, json>> &q_mian_first_list = v_caches[0].getresultsList();
    for (int i = 0; i < 4; i++)
    {
        unordered_map<string, list<pair<string, json>>::iterator> &q_map = v_spare_caches[i].getMap();
        q_map.clear();
        list<pair<string, json>> &q_list = v_spare_caches[i].getresultsList();
        q_list.clear();
        for (auto &it : q_mian_first_list)
        {
            v_spare_caches[i].addElement(it.first, it.second);
        }
        list<pair<string, json>> &q_spare_pending_list = v_spare_caches[i].getPendingUpdateList();
        q_spare_pending_list.clear();
    }
    cout << "update over!" << endl;
    cout << endl;
    v_spare_caches[0].writeToFile();
    // 将标志位设置为0，告诉lru对象完成所有更新，打开a的读写服务，打开b的写服务关闭b读服务
    pthread_rwlock_wrlock(&_rwlock);
    update_flag = 0;
    pthread_rwlock_unlock(&_rwlock);
    /* pthread_mutex_lock(&_mutex_lock); */
    /* update_flag = 0; */
    /* pthread_mutex_unlock(&_mutex_lock); */
}

// 将第一缓存中的数据更新到其他所有缓存
void CacheManager::UpdateAllCacheByFirst()
{//获取第一个已经更新好的，主LRUCache缓存的result List
    list<pair<string, json>> &temp_first = v_caches[0].getresultsList();
    //遍历插入四个备胎中的result_list中
    for (int i = 1; i < 4; i++)
    {

        unordered_map<string, list<pair<string, json>>::iterator> &q_map = v_caches[i].getMap();
        q_map.clear();
        list<pair<string, json>> &temp_other = v_caches[i].getresultsList();
        temp_other.clear();
        for (auto &it : temp_first)
        {
            v_caches[i].addElement(it.first, it.second);
        }
        list<pair<string, json>> &temp_other_pend = v_caches[i].getPendingUpdateList();
        temp_other_pend.clear();
    }
}
// 将四个待更新数据写入第一缓存中的result
void CacheManager::UpdateFirstCache(list<pair<string, json>> &update_list)
{
    for (auto &it : update_list)
    {
        json res = v_caches[0].getElementWithLRU(it.first);
        if (res.empty())
        {
            v_caches[0].addElement(it.first, it.second);
        }
    }
    update_list.clear();

}

// 获取所有待更新缓存的数量
int CacheManager::GetAllPendingUpdateListNum()
{
    int allnum = 0;
    for (int i = 0; i < 4; i++)
    {
        list<pair<string, json>> &temp = v_caches[i].getPendingUpdateList();
        allnum += temp.size();
    }
    return allnum;
}

//获取有用缓存的数量（公平性）
void CacheManager::GetUsefulNum(vector<int> &each_num)
{

    // 如果大约1000，计算出
    // 总数-1000=多余数
    // 多余数/总数=多余比例
    // 1-多余比例=有用比例
    // 有用比例*每个缓存的数量=应读取数量
    int allpendnum = GetAllPendingUpdateListNum();
    double excess_amount = allpendnum - 1000;
    double excess_ratio = excess_amount / allpendnum;
    double useful_ratio = 1 - excess_ratio;
    for (int i = 0; i < 4; i++)
    {
        list<pair<string, json>> &temp = v_caches[i].getPendingUpdateList();
        double useful_num = useful_ratio * temp.size();
        each_num.push_back((int)useful_num);
    }
}
//构造函数
CacheManager::CacheManager()
{
    /* pthread_mutex_init(&_mutex_lock, nullptr); */
    pthread_rwlock_init(&_rwlock, nullptr);
}
//析构函数
CacheManager::~CacheManager()
{
    /* pthread_mutex_destroy(&_mutex_lock); */
    pthread_rwlock_destroy(&_rwlock);
}
//返回读写锁的引用
pthread_rwlock_t &CacheManager::getRWlock()
{
    return _rwlock;
}
