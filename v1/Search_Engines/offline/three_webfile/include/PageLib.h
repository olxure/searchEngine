#ifndef OLX_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_PAGELIB_H_
#define OLX_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_PAGELIB_H_

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
        //getFiles()函数返回return _files;

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
    vector<string> _fileNames;//存储文件名的向量，_fileNames存的和files存的是一样的路径
    list<WebPage> _WebPage; // Use WebPage to store web information in list
};

#endif

