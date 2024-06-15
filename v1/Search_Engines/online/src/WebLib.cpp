#include "../include/WebLib.hpp"

#include <cmath>
#include <fstream>
#include <sstream>
#include "../include/tinyxml2.h"

using std::ifstream;
using std::istringstream;

using namespace tinyxml2;


WebLib* WebLib::_pInstance = nullptr;
pthread_once_t WebLib::_once = PTHREAD_ONCE_INIT;
function<void()> WebLib::_initFunc;

WebLib::WebLib(){}

WebLib::~WebLib(){}

WebLib* WebLib::getInstance(Configuration* conf)
{
    if(_pInstance == nullptr)
    {
        _initFunc = bind(loadConfiguration, conf);
    }
    pthread_once(&_once, init);
    return _pInstance;
}

void WebLib::init()
{
    _pInstance = new WebLib();
    _initFunc();
    atexit(destroy);
}

void WebLib::destroy()
{
    if(_pInstance)
    {
        delete _pInstance;
        _pInstance = nullptr;
    }
}

// 加载配置
void WebLib::loadConfiguration(Configuration* conf)
{
    map<string, string>& configMap = conf->getConfigMap();
    // TODO...加载三个文件
    for(auto& file : configMap)
    {
        if(file.first == "webOffsetLib.dat")
        {
            _pInstance->initOffsetLib(file.second);
        }
        else if(file.first == "invertIndexLib.dat")
        {
            _pInstance->initInvertIndexLib(file.second);
        }
        else if(file.first == "webPageLib.dat")
        {
            _pInstance->initWebPageLib(file.second);
        }
    }
    cout << "load config over!" << endl;
}

void WebLib::initOffsetLib(const string& filePath)
{
   ifstream ifs(filePath);
   if(!ifs)
   {
       cerr << "open file error: " << filePath << endl;
       return;
   }

   string line;
   int webId, webBegin, webLength;
   while(!ifs.eof())
   {
       getline(ifs, line);
       istringstream iss(line);
       iss >> webId >> webBegin >> webLength;
       _webOffsetLib[webId] = std::make_pair(webBegin, webLength);
   }

   ifs.close();
}

void WebLib::initInvertIndexLib(const string& filePath)
{
    ifstream ifs(filePath);
    if(!ifs)
    {
        cerr << "open file error: " << filePath << endl;
        return;
    }

    string line, word;
    int webId;
    double weight;
    while(!ifs.eof())
    {
        getline(ifs, line);
        istringstream iss(line);
        iss >> word;
        while(1)
        {
            iss >> webId >> weight;
            if(iss.eof())       // 因为即使后面剩一个空格或者换行，iss依然不会到末尾，此时需要再输出到变量中才能到末尾
            {
                break;
            }
            _invertIndexLib[word].emplace_back(std::make_pair(webId, weight));
        }
    }
    
    ifs.close();
}

void WebLib::initWebPageLib(const string& filePath)
{
    cout << "filePath" << filePath << endl;
    XMLDocument doc;
    int i = doc.LoadFile(filePath.c_str());
    cout << "i=" << i << endl;

    if (doc.LoadFile(filePath.c_str()) == XML_SUCCESS) {
        XMLElement* docElement = doc.FirstChildElement("doc");

        while (docElement != nullptr) {
            XMLElement* docidElement = docElement->FirstChildElement("docid");
            XMLElement* urlElement = docElement->FirstChildElement("url");
            XMLElement* titleElement = docElement->FirstChildElement("title");
            XMLElement* contentElement = docElement->FirstChildElement("content");

            int docId = -1;
            if(docidElement)
            {
                docId = atoi(docidElement->GetText());
                _webPageLib[docId]._docId = docId;
            }
            if (urlElement) {
                _webPageLib[docId]._docUrl = urlElement->GetText();
            }

            if (titleElement) {
                _webPageLib[docId]._docTitle = titleElement->GetText();
            }

            if (contentElement) {
                _webPageLib[docId]._docContent = contentElement->GetText();
            }

            docElement = docElement->NextSiblingElement("doc");
        }

    } else {
        cout << "Failed to load xml file." << endl;
        return;
    }
}


void WebLib::doAndQuery(const vector<string>& wordList, vector<int>& candidateId, map<string, pair<int, int>>& word_TF_DF)
{
    // 记录某篇文章id以及文章在wordlist词中出现的次数
    map<int, int> idOccurCount;
    int threshold = wordList.size();
    
    // 遍历倒排索引库，把每个词对应的文章id加入到idOccurCount中
    // 每有一个id对应的值就加一
    for(auto& elem : wordList)
    {
        auto iter = _invertIndexLib.find(elem);
        if(iter != _invertIndexLib.end())
        {
            for(auto& page : iter->second)
            {
                ++idOccurCount[page.first];
            }

            // DF=包含该词语的文档数量，因为搜索内容也算一篇文档，因此+1
            word_TF_DF[elem].second = iter->second.size() + 1;
        }
        else
        {
            // 若该词语在网页库中不存在，那它的DF=1，即搜索内容占一篇文档
            word_TF_DF[elem].second = 1;
        }

        // 同时把对应的单词出现次数记录一下,即TF
        // TF=该文章中出现的次数
        ++word_TF_DF[elem].first;
    }

    for(auto& elem : word_TF_DF)
    {
        cout << elem.first << " TF: " << elem.second.first << " DF: " << elem.second.second << endl;
    }

    // 遍历idOccurCount，若某篇文章的id等于threshold，那就满足条件
    for(auto& elem : idOccurCount)
    {
        if(elem.second == threshold)
        {
            cout << "candidateId: " << elem.first << endl;
            candidateId.push_back(elem.first);
        }
    }

}

map<string, double> WebLib::queryVector(const map<string, pair<int, int>>& word_TF_DF)
{
    map<string, double> result;
    map<string, double> wordWeight;
    // 因为搜索内容也算一篇文章，因此N需要加1
    size_t N = _webPageLib.size() + 1;

    double baseW = 0;
    // 遍历搜索词
    for(auto& word : word_TF_DF)
    {
        int TF = word.second.first;
        int DF = word.second.second;
        if(DF > (int)N - 1)
        {
            DF = N - 1;
        }
        double IDF = log2((double)N / DF + 1);
        cout << "IDF : " << IDF << " DF : " << DF << endl;
        double w = TF * IDF;

        wordWeight[word.first] = w;

        baseW += w*w;
    }
    /* cout << "baseW: " << baseW << endl; */

    for(auto& word : wordWeight)
    {
        double w_ = word.second / sqrt(baseW);
        cout << word.first << " w:" << w_ << endl;
        result[word.first] = w_;
    }

    return result;
}

void WebLib::sortPages(const map<string, pair<int, int>>& word_TF_DF, const vector<int>& candidateId, 
                       priority_queue<webPriority, vector<webPriority>, std::greater<struct webPriority>>& queue)
{
    if(candidateId.size() == 0)
    {
        return;
    }

    // 得到搜索词的各个权重——即搜索词的向量wordVector
    map<string, double> wordVector = queryVector(word_TF_DF);

    // 二维的向量坐标，第一个是网页的id
    map<int, vector<double>> pageVector;

    // 遍历每个搜索词，去倒排里面找对应的网页，找到该搜索词对应的网页id和w放到pageVector中
    // 其中每一个id对应一排权重，权重的值和搜索词一一对应
    for(auto& word : wordVector)
    {
        auto iter = _invertIndexLib.find(word.first);
        if(iter != _invertIndexLib.end())
        {
            // 肯定能找得到，candidateId里面存储的是候选网页
            for(auto& elem : candidateId)
            {
                // 找到对应id（elem）的权重，放到网页向量的对应位置
                double weight = findPageWeight(iter->second, elem);
                pageVector[elem].push_back(weight);
            }
        }
    }

    for(auto& id : pageVector)
    {
        cout << "id: " << id.first << " weight: ";
        for(auto& elem : id.second)
        {
            cout << elem << " "; 
        }
        cout << endl;
    }
    
    // 网页的向量也得到了，此时把单词向量变成一个vector
    vector<double> searchVector;
    auto priorityIter = wordVector.begin();     //优先级最高的单词,留给摘要使用

    for(auto iter = wordVector.begin(); iter != wordVector.end(); ++iter)
    {
        if(iter->second > priorityIter->second)
        {
            priorityIter = iter;            // 找到优先级最高的单词
        }
        searchVector.push_back(iter->second);
    }

    cosAlgorithm(pageVector, searchVector, priorityIter->first, queue);
}

void WebLib::cosAlgorithm(map<int, vector<double>>& pageVector, vector<double>& searchVector, string priorityWord,
                          priority_queue<webPriority, vector<webPriority>, std::greater<struct webPriority>>& queue)
{
    // 网页包含网页id，与查询内容的夹角，本次查询优先级最高的单词
    webPriority web;
    
    // y的绝对值，y是要查询内容的向量
    double yAbsoluteValue = getAbsoluteValue(searchVector);
    // x的绝对值，x和y的点积，x是网页向量
    double xAbsoluteValue, cross;

    web.word = priorityWord;

    for(auto& page : pageVector)
    {
        // 记录网页id
        web.id = page.first;
        // 计算网页向量的绝对值
        xAbsoluteValue = getAbsoluteValue(page.second);
        cross = crossVector(searchVector, page.second);

        // 计算网页和查询内容向量的夹角
        web.cosAngle = cross / (xAbsoluteValue * yAbsoluteValue);

        cout << web.id << " " << web.cosAngle << endl;

        // 把该网页的信息插入到优先级队列中
        queue.emplace(web);
    }
}

double WebLib::crossVector(vector<double>& x, vector<double>& y)
{
    double result = 0;
    
    // 计算x和y向量的点积
    for(size_t i = 0; i < x.size(); ++i)
    {
        result += x[i]*y[i];
    }

    return result;
}

double WebLib::getAbsoluteValue(vector<double>& vec)
{
    double result = 0;
    
    // 计算向量的绝对值
    for(auto& elem : vec)
    {
        result += elem * elem;
    }

    return sqrt(result);
}

double WebLib::findPageWeight(vector<pair<int, double>>& pageWeight, int id)
{
    // 找到网页id对应的权重（已经确定查询词）
    for(auto& elem : pageWeight)
    {
        if(id == elem.first)
        {
            return elem.second;
        }
    }
    cout << "can't find id?" << endl;
    return -1;
}

unordered_map<int, WebPage>& WebLib::getWebPage()
{
    return _webPageLib;
}
