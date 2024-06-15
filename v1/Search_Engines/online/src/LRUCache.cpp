#include "../include/LRUCache.h"

#include <memory>

unordered_map<string, list<pair<string, json>>::iterator>& LRUCache::getMap() {
    return _hashMap;
}

LRUCache::LRUCache(const string& filePath, int num) : _filePath(filePath), _capacity(num) {
    readFromFile();

    // Initialize _hashMap
    for (auto it = _resultsList.begin(); it != _resultsList.end(); ++it) {
        _hashMap[it->first] = it;
    }
}

json LRUCache::getElementWithLRU(const string& key) {
    // LRU algorithm
    auto iter = _hashMap.find(key);

    if (iter != _hashMap.end()) {
        // If key exists, move the node to the head of the list
        auto iter = _hashMap[key];
        moveToHead(iter);
        return iter->second; // Return the value
    }

    json emptyJson;
    return emptyJson;
}

json LRUCache::getElementWithoutLRU(const string& key) {
    // LRU algorithm
    auto iter = _hashMap.find(key);

    if (iter != _hashMap.end()) {
        // Push the element to the message sending queue
    } else {
        return -1;
    }

    return 0;
}

void LRUCache::addElement(const string& key, const json& value) {
    // LRU algorithm
    if (static_cast<int>(_resultsList.size()) >= _capacity) {
        // If the cache is full, remove the tail node and corresponding hash map entry
        auto lastNode = _resultsList.back();
        _hashMap.erase(lastNode.first);
        _resultsList.pop_back();
    }

    // Create a new node and insert it at the head of the list
    _resultsList.emplace_front(key, value);
    // Update the iterator in the hash map
    _hashMap[key] = _resultsList.begin();

    // Add the new node to the pending update list
    _pendingUpdateList.emplace_front(key, value);
}

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
                json jsonObj = json::parse(jsonString); // Parse the string into a JSON object
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

void LRUCache::writeToFile() {
    std::ofstream outputFile(_filePath);
    if (outputFile.is_open()) {
        for (const auto& pair : _resultsList) {
            std::string jsonString = pair.second.dump(); // Convert the JSON object to a string
            outputFile << pair.first << " " << jsonString << "\n";
        }
        outputFile.close();
    } else {
        // Error opening file
    }
}

list<pair<string, json>>& LRUCache::getresultsList() {
    return _resultsList;
}

list<pair<string, json>>& LRUCache::getPendingUpdateList() {
    return _pendingUpdateList;
}

void LRUCache::moveToHead(std::list<std::pair<std::string, json>>::iterator iter) {
    auto node = *iter;
    _resultsList.erase(iter);
    _resultsList.emplace_front(node);
    _hashMap[node.first] = _resultsList.begin();
}

void LRUCache::print() {
    std::cout << "LRUList: " << std::endl;
    for (auto iter = _resultsList.begin(); iter != _resultsList.end(); ++iter) {
        std::cout << "iter->first: " << iter->first << std::endl;
    }
}

LRUCache::LRUCache(const LRUCache& cache) {
    _filePath = cache._filePath;
    _num = cache._num;
    _capacity = cache._capacity;

    // Deep copy _resultsList
    for (const auto& pair : cache._resultsList) {
        _resultsList.push_back(pair);
    }

    // Update _hashMap to point to the new _resultsList iterator
    for (auto iter = _resultsList.begin(); iter != _resultsList.end(); ++iter) {
        _hashMap[iter->first] = iter;
    }

    // Deep copy _pendingUpdateList
    for (const auto& pair : cache._pendingUpdateList) {
        _pendingUpdateList.push_back(pair);
    }
}

