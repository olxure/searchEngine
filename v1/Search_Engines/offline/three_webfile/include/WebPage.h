#ifndef OLX_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_WEBPAGE_H_
#define OLX_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_WEBPAGE_H_

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

class WebPage {
    friend class PageLib;
    friend class FileProcessor;

public:
    WebPage() {
        // Default constructor
    }

    // Member variables
    string _doc;
    int _docId;         // ID of the web page in the web page library
    string _docTitle;   // Title of the web page in the web page library
    string _docUrl;     // URL of the web page in the web page library
    string _docContent; // Content of the web page in the web page library

    // Print WebPage details (for testing purposes)
    void WebPagePrint() {
        cout << "_docId: " << _docId
             << "  _docTitle: " << _docTitle
             << "  __docUrl: " << _docUrl
             << "  _docContent: " << _docContent
             << endl;
    }
};

#endif

