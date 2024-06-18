#include "../include/WebPageQuery.hpp"

WebPageQuery::WebPageQuery(Configuration* conf)
{
    _webLib = WebLib::getInstance(conf);
}//通过WebPageQuery类的构造函数来初始化WebPageQuery类的私有数据成员_webLib，类型是WebLib* _webLib;

WebPageQuery::~WebPageQuery()
{}
//参数 wordList 是一个包含搜索关键词的 vector<string>。
void WebPageQuery::doQuery(vector<string>& wordList)
{
    vector<int> candidateId;//candidateId：一个整型向量，用于存储候选网页的 ID

    //word_TF_DF：一个映射，键是字符串（关键词），值是一个包含两个整型的 pair，分别表示词频（TF）和文档频率（DF）。
    map<string, pair<int, int>> word_TF_DF;//

    // 执行 AND 查询，将结果存储到 candidateId 和 word_TF_DF 中
    _webLib->doAndQuery(wordList, candidateId, word_TF_DF);
    
    // 对查询结果进行排序，将排序后的网页存储到优先级队列 _queue 中
    _webLib->sortPages(word_TF_DF, candidateId, _queue);
    
    cout << "doQuery over!" << endl;// 输出查询结束信息
}

void WebPageQuery::handleAbstract(){

    cout << "handle begin" << endl;
    // 如果优先队列 _queue 为空，则输出提示信息并返回
    if(_queue.empty())
    {
        cout << "the result is empty" << endl;
        return;
    }

    // 获取网页库的引用
    unordered_map<int, WebPage>& webPages = _webLib->getWebPage();  // 网页库
    
    string keyword = _queue.top().word;// 获取优先队列中的关键词
    cout << "keyword: " << keyword << endl << endl;

    // 将优先队列中的网页 ID 添加到 _pages 向量中，并清空优先队列
    while(!_queue.empty())
    {
        _pages.push_back(webPages[_queue.top().id]);
        _queue.pop();
    }

    // 定义一个用于字符串转换的对象
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    // 遍历 _pages 向量中的每个网页
    for(WebPage& page : _pages){

    //string -> wstring
        // 将 _docTitle 和 _docContent 转换为宽字符串并拼接
        std::wstring summary = converter.from_bytes(page._docTitle);
            summary.append(converter.from_bytes(page._docContent));
        std::wstring wkeyword = converter.from_bytes(keyword);

    //init summary
        // 查找关键词在 summary 中的位置
        auto kwindex = summary.find(wkeyword);
        if(kwindex == string::npos)
            cout << "no find" << endl;

         // 查找关键词前的最后一个换行符的位置
        auto begin = summary.find_last_of(L'\n',kwindex);
        if(begin == string::npos)
            begin = 0;

        // 根据关键词的位置截取 summary 的部分内容作为摘要
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
        auto iter = summary.find(L"\n",0);// 将摘要中的换行符替换为空格
        
        if(iter != std::string::npos)
            summary.replace(iter,1,L"");
        
        iter = summary.find(L"\n",iter);
        
        if(iter != std::string::npos)
            summary.replace(iter,1,L" ");
        
        // insert '/n' ".."// 在摘要中间插入换行符，并在末尾添加省略号
        summary.insert(summary.size()/2,L"\n");
        summary.replace(summary.size()-3,3,L"...");
        
        // 将摘要转换回窄字符串并赋值给 _docContent
        page._docContent = converter.to_bytes(summary);

        // 输出处理后的页面信息息
        cout << "pageid: " << page._docId << " content: " << page._docContent << endl << endl;
    }
}

void WebPageQuery::generateJson(/*const vector<webpage>& _pages,json& _finalResult*/) {
    // 遍历 _pages 向量中的每个页面对象
    for (const auto& page : _pages) {
        json pageJson;// 创建一个空的 JSON 对象
        try {
            // 在将页面属性添加到 JSON 之前检查它们是否有效
            if (!page._docTitle.empty() && !page._docUrl.empty() && !page._docContent.empty()) {
                // 将页面属性添加到 JSON 对象
                pageJson["_docTitle"] = page._docTitle;
                pageJson["_docUrl"] = page._docUrl;
                pageJson["_docSummary"] = page._docContent;

                // 将 JSON 对象添加到 _finalResult 数组中
                _finalResult.push_back(pageJson);
            } else {
                // 输出警告信息，跳过无效的页面
                std::cerr << "Warning: Skipping a page with invalid attributes." << std::endl;
                std::cerr << "Title: " << page._docTitle << ", URL: " << page._docUrl << ", Content: " << page._docContent << std::endl;
            }
        } catch (const std::exception& e) {
            // 捕获并输出生成 JSON 过程中发生的异常
            std::cerr << "Error generating JSON for a page: " << e.what() << std::endl;
        }//std::exception 类是 C++ 标准库中所有异常类的基类。它定义了一些基本的接口，包括 what() 函数，用于获取描述异常的字符串
        //what() 函数是 std::exception 类中的一个虚函数，返回一个指向 C 风格字符串的指针。这个字符串通常包含对异常的描述性信息，并将其输出到标准错误流
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

