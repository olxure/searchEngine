#include "../include/WebPageQuery.hpp"

WebPageQuery::WebPageQuery(Configuration* conf)
{
    _webLib = WebLib::getInstance(conf);
}

WebPageQuery::~WebPageQuery()
{}

void WebPageQuery::doQuery(vector<string>& wordList)
{
    vector<int> candidateId;
    map<string, pair<int, int>> word_TF_DF;

    _webLib->doAndQuery(wordList, candidateId, word_TF_DF);
    
    _webLib->sortPages(word_TF_DF, candidateId, _queue);
    
    cout << "doQuery over!" << endl;
}

void WebPageQuery::handleAbstract(){

    cout << "handle begin" << endl;
    if(_queue.empty())
    {
        cout << "the result is empty" << endl;
        return;
    }

    unordered_map<int, WebPage>& webPages = _webLib->getWebPage();  // 网页库
    
    string keyword = _queue.top().word;
    cout << "keyword: " << keyword << endl << endl;

    while(!_queue.empty())
    {
        _pages.push_back(webPages[_queue.top().id]);
        _queue.pop();
    }

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    for(WebPage& page : _pages){

    //string -> wstring
        std::wstring summary = converter.from_bytes(page._docTitle);
            summary.append(converter.from_bytes(page._docContent));
        std::wstring wkeyword = converter.from_bytes(keyword);
    //init summary
        auto kwindex = summary.find(wkeyword);
        if(kwindex == string::npos)
            cout << "no find" << endl;
        auto begin = summary.find_last_of(L'\n',kwindex);

        if(begin == string::npos)
            begin = 0;
        if((kwindex-begin)>60){
            if((summary.size()-kwindex)<60){
                kwindex=kwindex-60+(summary.size()-kwindex);
            }
            summary = summary.substr(kwindex,60);

        }else if((summary.size()-begin)<60){
            auto len = summary.size()-begin;
            begin =begin-60+len; 
            summary = summary.substr(begin,60);
        }else{

            summary=summary.substr(begin,60);
        }
        
        // '/n'-> ' '
        auto iter = summary.find(L"\n",0);
        
        if(iter != std::string::npos)
            summary.replace(iter,1,L"");
        
        iter = summary.find(L"\n",iter);
        
        if(iter != std::string::npos)
            summary.replace(iter,1,L" ");
        
        // insert '/n' ".."
        summary.insert(summary.size()/2,L"\n");
        summary.replace(summary.size()-3,3,L"...");
        
        page._docContent = converter.to_bytes(summary);

        cout << "pageid: " << page._docId << " content: " << page._docContent << endl << endl;
    }
}

void WebPageQuery::generateJson(/*const vector<webpage>& _pages,json& _finalResult*/) {
    for (const auto& page : _pages) {
        json pageJson;
        try {
            // Check if page attributes are valid before adding them to JSON
            if (!page._docTitle.empty() && !page._docUrl.empty() && !page._docContent.empty()) {
                pageJson["_docTitle"] = page._docTitle;
                pageJson["_docUrl"] = page._docUrl;
                pageJson["_docSummary"] = page._docContent;

                _finalResult.push_back(pageJson);
            } else {
                std::cerr << "Warning: Skipping a page with invalid attributes." << std::endl;
                std::cerr << "Title: " << page._docTitle << ", URL: " << page._docUrl << ", Content: " << page._docContent << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error generating JSON for a page: " << e.what() << std::endl;
        }
    }
}


//生成json有点问题，暂存
// void WebPageQuery::generateJson(/*const vector<webpage>& _pages,json& _finalResult*/){
//     for(const auto& page : _pages){
//         json pageJson;
//         //pageJson["_doc"] = page._doc;
//         //pageJson["_docId"] = page._docId;
//         pageJson["_docTitle"] = page._docTitle;
//         pageJson["_docUrl"] = page._docUrl;
//         //pageJson["_docContent"] = page._docContent;
//         pageJson["_docSummary"] = page._docContent;

//         _finalResult.push_back(pageJson);
//     }
// }


json WebPageQuery::getResult(){
    return _finalResult;
}

