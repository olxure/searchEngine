#ifndef WD_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_FILEPROCESSOR_H_
#define WD_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_FILEPROCESSOR_H_

#include <string>
#include <vector>
#include "PageLib.h"

using std::string;
using std::vector;

class PageLib;

class FileProcessor {
public:
    FileProcessor(PageLib& pageLib)
        : _pageLib(pageLib) {
        // Constructor
    }

    // Create file
    void create() {
        vector<string> fileNames = _pageLib.getFiles();

        for (const string& fileName : fileNames) {
            process(fileName);
        }
    }

    // Process file with given file name
    int process(const string& fileName);

private:
    PageLib& _pageLib;  // Reference to the PageLib object
};

#endif

