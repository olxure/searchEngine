#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <iostream>
#include <string>

using std::string;
using std::cout;
using std::endl;

class WebPage
{
public:
    WebPage()
    {}

    // WebPage(const string& doc)
    // : _doc(doc)
    // {
    //     
    // }
    
    void WebPagePrint() {
        cout << "_docId:" << _docId
            << "  _docTitle:" << _docTitle
            << "  __docUrl:" << _docUrl
            << "  _docContent:" << _docContent
            << endl;
    }

    int _docId;
    string _docTitle;
    string _docUrl;
    string _docContent;
};

#endif
