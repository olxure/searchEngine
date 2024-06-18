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
class LRUCache {
public:
    /**
     * Constructor for the LRUCache class.
     * @param filename The name of the file to read/write cache data.
     * @param num The maximum number of elements in the cache.
     */
    LRUCache(const string& filename, int num = 1000);

    /**
     * Copy constructor for the LRUCache class.
     * @param cache The LRUCache object to be copied.
     */
    LRUCache(const LRUCache& cache);

    /**
     * Get the value associated with the given key from the cache using LRU policy.
     * @param key The key of the element to retrieve.
     * @return The JSON value associated with the key, or an empty JSON object if not found.
     */
    json getElementWithLRU(const string& key);

    /**
     * Get the value associated with the given key from the cache without using LRU policy.
     * @param key The key of the element to retrieve.
     * @return The JSON value associated with the key, or an empty JSON object if not found.
     */
    json getElementWithoutLRU(const string& key);

    /**
     * Add a key-value pair to the cache.
     * @param key The key of the element to add.
     * @param value The JSON value to be added.
     */
    void addElement(const string& key, const json& value);

    /**
     * Read cache data from the file.
     */
    void readFromFile();

    /**
     * Write cache data to the file.
     */
    void writeToFile();

    /**
     * Update the cache with the contents of another cache object.
     * @param rhs The cache object to be merged.
     */
    void update(const LRUCache& rhs);

    /**
     * Get the results list of the cache.
     * @return Reference to the results list.
     */
    list<pair<string, json>>& getresultsList();

    /**
     * Get the pending update list of the cache.
     * @return Reference to the pending update list.
     */
    list<pair<string, json>>& getPendingUpdateList();

    /**
     * Get the hash map of the cache.
     * @return Reference to the hash map.
     */
    unordered_map<string, list<pair<string, json>>::iterator>& getMap();

    /**
     * Print the contents of the cache.
     */
    void print();

private:
    /**
     * Move the element pointed by the iterator to the head of the results list.
     * @param iter Iterator pointing to the element to be moved.
     */
    void moveToHead(std::list<std::pair<std::string, json>>::iterator iter);

private:
    string _filePath; // File path for reading/writing cache data
    int _num; // Maximum number of elements in the cache
    unordered_map<string, list<pair<string, json>>::iterator> _hashMap; // Hash map for quick lookup
    list<pair<string, json>> _resultsList; // Results list for LRU policy
    list<pair<string, json>> _pendingUpdateList; // Pending update list
    int _capacity; // Current capacity of the cache
};

#endif

