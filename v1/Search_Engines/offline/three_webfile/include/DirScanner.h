#ifndef WD_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_DIRSCANNER_H_
#define WD_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_DIRSCANNER_H_

// Consider using <filesystem> for cross-platform compatibility (C++17)
#include "../include/Configuration.h"

#include <vector>
#include <string>
#include <dirent.h>
#include <unistd.h>

using std::string;
using std::vector;

class DirScanner {
public:
    DirScanner() {}

    void operator()() {
        Configuration& config = Configuration::getInstance("");  // Pass an empty string here as it doesn't affect the behavior
        map<string, string>& configMap = config.getConfigMap();

        // Add error handling here

        string key_dir = "dirname";
        string dirname = configMap[key_dir];

        traverse(dirname);
    }

    vector<string>& getFiles() {
        return _files;
    }

private:
    // Traverse all files under the specified directory (dirname) and store their absolute paths in _files
    void traverse(const string& dirname) {
        DIR* dir = opendir(dirname.c_str());
        // Error handling
        if (dir == nullptr) {
            std::cerr << "Failed to open directory: " << dirname << std::endl;
            return;
        }

        dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string fileName = entry->d_name;

            // Ignore current directory and parent directory
            if (fileName == "." || fileName == "..") {
                continue;
            }

            std::string filePath = dirname + "/" + fileName;
            if (entry->d_type == DT_DIR) {
                // Recursively traverse subdirectories
                traverse(filePath);
            } else if (entry->d_type == DT_REG) {
                // Store the file path in the vector
                _files.push_back(filePath);
            }
        }

        closedir(dir);
    }

private:
    vector<string> _files;
};

#endif

