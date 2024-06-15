#ifndef WD_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_PAGELIB_H_
#define WD_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_PAGELIB_H_

#include "../include/DirScanner.h"
#include "../include/WebPage.h"

#include <vector>
#include <string>
#include <map>
#include <utility>
#include <memory>
#include <list>

using std::map;
using std::pair;
using std::string;
using std::vector;
using std::list;

class DirScanner;

class PageLib {
public:
    // Constructor
    PageLib(DirScanner& dirScanner)
        : _dirScanner(dirScanner), _fileNames(_dirScanner.getFiles()) {}

    // Getter for file names
    vector<string>& getFiles() {
        return _fileNames;
    }

    // Getter for WebPage objects
    list<WebPage>& getWebPage() {
        return _WebPage;
    }

private:
    DirScanner& _dirScanner;
    vector<string> _fileNames;
    list<WebPage> _WebPage; // Use WebPage to store web information in list
};

#endif

