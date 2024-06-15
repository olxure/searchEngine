#ifndef WD_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_CONFIGURATION_H_
#define WD_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_CONFIGURATION_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>

using std::map;
using std::set;
using std::string;

class Configuration {
public:
    // Get the singleton instance (lazy initialization)
    static Configuration& getInstance(const string& filepath) {
        static Configuration instance(filepath);
        return instance;
    }

    // Get the configuration map
    map<string, string>& getConfigMap() {
        return _configMap;
    }

    // Get the stop word list
    set<string>& getStopWordList() {
        return _stopWordList;
    }

private:
    Configuration(const string& filepath)
        : _filepath(filepath) {
        std::ifstream configFile(_filepath);
        if (configFile.is_open()) {
            string line;
            while (std::getline(configFile, line)) {
                std::istringstream iss(line);
                string key, value;
                if (iss >> key >> value) {
                    _configMap[key] = value;
                }
            }

            configFile.close();
        } else {
            std::cout << "Failed to open config file." << std::endl;
            throw std::runtime_error("Error processing configuration file");
        }
    }

    ~Configuration() {}

private:
    string _filepath;               // Configuration file path
    map<string, string> _configMap;  // Configuration file content
    set<string> _stopWordList;       // Stop word list
};

#endif

