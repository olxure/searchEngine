#ifndef OLX_PROJECT_SEARCH_ENGINES_ONLINE_INCLUDE_LRUCACHE_H_
#define OLX_PROJECT_SEARCH_ENGINES_ONLINE_INCLUDE_LRUCACHE_H_

#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <nlohmann/json.hpp>

using std::list;
using std::pair;
using std::string;
using std::unordered_map;

using json = nlohmann::json;

// LRUCache class represents a least recently used cache
// LRUCache 类表示最近最少使用的缓存
class LRUCache {
public:
    /**
     * LRUCache 类的构造函数
     * Constructor for the LRUCache class.
     * @param filename The name of the file to read/write cache data.读/写缓存数据的文件名
     * @param num The maximum number of elements in the cache.缓存中的最大元素数量
     */
    LRUCache(const string& filename, int num = 1000);

    /**
     *  LRUCache 类的拷贝构造函数
     * Copy constructor for the LRUCache class.
     * @param cache The LRUCache object to be copied. 要复制的 LRUCache 对象
     */
    LRUCache(const LRUCache& cache);

    /**
     * 使用 LRU 策略从缓存中获取与给定键关联的值
     * Get the value associated with the given key from the cache using LRU policy.要检索的元素的键
     * @param key The key of the element to retrieve.
     * @return The JSON value associated with the key, or an empty JSON object if not found.
     * return 与键关联的 JSON 值，如果未找到则返回空的 JSON 对象
     */
    json getElementWithLRU(const string& key);

    /**
     * 不使用 LRU 策略从缓存中获取与给定键关联的值
     * Get the value associated with the given key from the cache without using LRU policy.
     * @param key The key of the element to retrieve.要检索的元素的键
     * @return The JSON value associated with the key, or an empty JSON object if not found.
     * return 与键关联的 JSON 值，如果未找到则返回空的 JSON 对象
     */
    json getElementWithoutLRU(const string& key);

    /**
     * 向缓存中添加键值对
     * Add a key-value pair to the cache.
     * @param key The key of the element to add.要添加的元素的键
     * @param value The JSON value to be added.要添加的 JSON 值
     */
    void addElement(const string& key, const json& value);

    /**
     * Read cache data from the file.从文件中读取缓存数据
     */
    void readFromFile();

    /**
     * Write cache data to the file.将缓存数据写入文件
     */
    void writeToFile();

    /**
     * Update the cache with the contents of another cache object.使用另一个缓存对象的内容更新缓存
     * @param rhs The cache object to be merged.要合并的缓存对象
     */
    void update(const LRUCache& rhs);

    /**
     * Get the results list of the cache.获取缓存的结果列表
     * @return Reference to the results list.结果列表的引用
     */
    list<pair<string, json>>& getresultsList();

    /**
     * Get the pending update list of the cache.获取缓存的待更新列表
     * @return Reference to the pending update list.待更新列表的引用
     */
    list<pair<string, json>>& getPendingUpdateList();

    /**
     * Get the hash map of the cache.获取缓存的哈希映射
     * @return Reference to the hash map.哈希映射的引用
     */
    unordered_map<string, list<pair<string, json>>::iterator>& getMap();

    /**
     * Print the contents of the cache.印缓存的内容
     */
    void print();

private:
    /**
     * Move the element pointed by the iterator to the head of the results list.
     * 将迭代器指向的元素移动到结果列表的头部
     * @param iter Iterator pointing to the element to be moved.指向要移动的元素的迭代器
     */
    void moveToHead(std::list<std::pair<std::string, json>>::iterator iter);

private:
    string _filePath; // File path for reading/writing cache data// 读/写缓存数据的文件路径
    int _num; // Maximum number of elements in the cache// 缓存中的最大元素数量
    unordered_map<string, list<pair<string, json>>::iterator> _hashMap; // Hash map for quick lookup// 用于快速查找的哈希映射
    list<pair<string, json>> _resultsList; // Results list for LRU policy// 结果列表，用于 LRU 策略
    list<pair<string, json>> _pendingUpdateList; // Pending update list// 待更新列表
    int _capacity; // Current capacity of the cache// 缓存的当前容量
};

#endif

