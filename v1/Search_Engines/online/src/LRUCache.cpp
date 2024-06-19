#include "../include/LRUCache.h"

#include <memory>

// 获取哈希映射表，返回一个引用
unordered_map<string, list<pair<string, json>>::iterator>& LRUCache::getMap() {
    return _hashMap;
}

// LRUCache构造函数，初始化文件路径和容量，并从文件中读取数据
LRUCache::LRUCache(const string& filePath, int num) : _filePath(filePath), _capacity(num) {
    readFromFile();

    // 初始化哈希映射表
    for (auto it = _resultsList.begin(); it != _resultsList.end(); ++it) {
        _hashMap[it->first] = it;
    }
}

// 根据LRU算法获取元素，如果元素存在，将其移动到链表头部
json LRUCache::getElementWithLRU(const string& key) {
    // LRU algorithm
    auto iter = _hashMap.find(key);

    if (iter != _hashMap.end()) {
        // If key exists, move the node to the head of the list
        // 如果key存在，将节点移动到链表头部
        auto iter = _hashMap[key];
        moveToHead(iter);
        return iter->second; // Return the value
    }

    json emptyJson;
    return emptyJson;
}

// 获取元素但不使用LRU算法
json LRUCache::getElementWithoutLRU(const string& key) {
    // LRU algorithm
    auto iter = _hashMap.find(key);

    if (iter != _hashMap.end()) {
        // Push the element to the message sending queue
        // 将元素推送到消息发送队列
    } else {
        return -1;
    }

    return 0;
}

// 向缓存中添加元素
void LRUCache::addElement(const string& key, const json& value) {
    // LRU algorithm
    if (static_cast<int>(_resultsList.size()) >= _capacity) {
        // If the cache is full, remove the tail node and corresponding hash map entry
        // 如果缓存已满，删除尾部节点及其对应的哈希表条目
        auto lastNode = _resultsList.back();
        _hashMap.erase(lastNode.first);
        _resultsList.pop_back();
    }

    // Create a new node and insert it at the head of the list
    // 创建新节点并将其插入到链表头部
    _resultsList.emplace_front(key, value);

    // Update the iterator in the hash map
    // 更新哈希表中的迭代器
    _hashMap[key] = _resultsList.begin();

    // Add the new node to the pending update list
    // 将新节点添加到待更新列表
    _pendingUpdateList.emplace_front(key, value);
}

// 从文件中读取数据
void LRUCache::readFromFile() {
    std::ifstream inputFile(_filePath);
    if (inputFile.is_open()) {
        std::string line;
        int maxLen = 0;
        while (std::getline(inputFile, line)) {
            if (maxLen == _capacity)
                break;
            std::istringstream iss(line);
            std::string key, jsonString;
            if (iss >> key >> jsonString) {
                json jsonObj = json::parse(jsonString); // Parse the string into a JSON object// 将字符串解析为JSON对象
                _resultsList.emplace_back(std::make_pair(key, jsonObj));
                ++maxLen;
            }
        }
        inputFile.close();
        std::cout << "Data has been read from file: " << _filePath << std::endl;
    } else {
        std::cerr << "Unable to open file: " << _filePath << std::endl;
    }
}

// 将数据写入文件
void LRUCache::writeToFile() {
    std::ofstream outputFile(_filePath);
    if (outputFile.is_open()) {
        for (const auto& pair : _resultsList) {
            std::string jsonString = pair.second.dump(); // Convert the JSON object to a string// 将JSON对象转换为字符串
            outputFile << pair.first << " " << jsonString << "\n";
        }
        outputFile.close();
    } else {
        // Error opening file // 打开文件错误
    }
}

// 获取结果列表
list<pair<string, json>>& LRUCache::getresultsList() {
    return _resultsList;
}

// 获取待更新列表
list<pair<string, json>>& LRUCache::getPendingUpdateList() {
    return _pendingUpdateList;
}

// 将节点移动到链表头部
void LRUCache::moveToHead(std::list<std::pair<std::string, json>>::iterator iter) {
    auto node = *iter;
    _resultsList.erase(iter);
    _resultsList.emplace_front(node);
    _hashMap[node.first] = _resultsList.begin();
}

// 打印LRU列表
void LRUCache::print() {
    std::cout << "LRUList: " << std::endl;
    for (auto iter = _resultsList.begin(); iter != _resultsList.end(); ++iter) {
        std::cout << "iter->first: " << iter->first << std::endl;
    }
}

// LRUCache的拷贝构造函数
LRUCache::LRUCache(const LRUCache& cache) {
    _filePath = cache._filePath;
    _num = cache._num;
    _capacity = cache._capacity;

    // Deep copy _resultsList// 深拷贝_resultsList
    for (const auto& pair : cache._resultsList) {
        _resultsList.push_back(pair);
    }

    // Update _hashMap to point to the new _resultsList iterator// 更新_hashMap指向新的_resultsList迭代器
    for (auto iter = _resultsList.begin(); iter != _resultsList.end(); ++iter) {
        _hashMap[iter->first] = iter;
    }

    // Deep copy _pendingUpdateList// 深拷贝_pendingUpdateList
    for (const auto& pair : cache._pendingUpdateList) {
        _pendingUpdateList.push_back(pair);
    }
}

