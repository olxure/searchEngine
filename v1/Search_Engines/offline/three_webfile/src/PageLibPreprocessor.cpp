#include "../include/PageLibPreprocessor.h"

// 将v_webpage去重
void PageLibPreprocessor::cutRedundantPages()
{
    simhash::Simhasher simhasher("../include/cppjieba/dict/jieba.dict.utf8",
                                 "../include/cppjieba/dict/hmm_model.utf8",
                                 "../include/cppjieba/dict/idf.utf8",
                                 "../include/cppjieba/dict/stop_words.utf8");
    size_t topN = 5;        // 关键词设置为5个
    vector<uint64_t> hashVec;       // 用于存储simhash值
    hashVec.reserve(l_webpage.size());  // 提前分配好空间

    int i = 0, flag = 0;

    for(auto& elem : l_webpage)
    {
        simhasher.make(elem._docContent, topN, hashVec[i]);     //使用simhash计算每篇文章的hash值存到hashVec中
        for(int j = 0; j < i; ++j)           // 每次生成一个新的simhash值就和原有的simhash值进行比较
        {
            if(simhash::Simhasher::isEqual(hashVec[i], hashVec[j])) // 如果新生成的simhash值与原来的相等，flag=1
            {
                flag = 1;
                break;
            }
        }

        if(flag != 1)       // flag=1的话，就丢弃该simhash值和对应的文章
        {                   // 如果flag=0的话，就更新此时文章id为i+1，并把它放到v_webpage中
            ++i;
            elem._docId = i;
            v_webpage.push_back(elem);
        }
        flag = 0;
    }

}

// 建立倒排索引库
void PageLibPreprocessor::buildInvertIndexTable()
{
    map<string, int> word_allnumber;
    // 第一步：初始化_stringinfo_and_wordinfo
    for (auto &v_webpage_it : v_webpage)
    {
        AllInfo temp;
        temp.content = v_webpage_it._docTitle + v_webpage_it._docContent;
        temp.all_w = 0;
        _stringinfo_and_wordinfo.insert(make_pair(v_webpage_it._docId, temp));
    }
    //  第二步：中文分词
    for (auto &s_and_w_it : _stringinfo_and_wordinfo)
    {
        // 获取一篇文章中的单词以及其在文章中出现的次数
        map<string, int> temp = _cppJieba.getWordFrequency(s_and_w_it.second.content);
        for (auto &temp_it : temp)
        {
            // 将单词的数量添加至“_stringinfo_and_wordinfo”数据结构中
            s_and_w_it.second.wordinfo.insert(make_pair(temp_it.first, make_pair(temp_it.second, 0)));
            // 将单词的数量添加至“map<string, int> word_allnumber”数据结构中
            auto word_allnumber_it = word_allnumber.find(temp_it.first);
            if (word_allnumber_it == word_allnumber.end())
            {
                word_allnumber.insert(make_pair(temp_it.first, 1));
            }
            else
            {
                word_allnumber_it->second += 1;
            }
        }
    }
    // 将英语单词分词，并将其存入“_stringinfo_and_wordinfo”与“word_allnumber”数据结构中
    map<string, bool> isinput;
    for (auto &s_and_w_it : _stringinfo_and_wordinfo)
    {
        isinput.erase(isinput.begin(), isinput.end());
        // 英文单词存储的数量
        char word[128];
        memset(word, 0, sizeof(word));
        // 英文单词的长度
        int word_num = 0;
        // 获取英文单词
        for (int i = 0; i < s_and_w_it.second.content.length(); i++)
        {
            // 添加字母
            if (isalpha(s_and_w_it.second.content[i]))
            {

                if (isupper(s_and_w_it.second.content[i]))
                {
                    word[word_num] = tolower(s_and_w_it.second.content[i]);
                }
                else
                {
                    word[word_num] = s_and_w_it.second.content[i];
                }
                word_num++;
            }

            if (word_num != 0 && ((!isalpha(s_and_w_it.second.content[i])) || (i == s_and_w_it.second.content.length() - 1)))
            {
                // 将字母填入“_stringinfo_and_wordinfo”数据结构中
                auto tt = s_and_w_it.second.wordinfo.find(word);
                if (tt == s_and_w_it.second.wordinfo.end())
                {
                    s_and_w_it.second.wordinfo.insert(make_pair(word, make_pair(1, 0)));
                }
                else
                {
                    tt->second.first++;
                }
                // 将数据填入“ word_allnumber”数据结构中
                auto word_allnumber_it = word_allnumber.find(word);
                if (word_allnumber_it == word_allnumber.end())
                {
                    word_allnumber.insert(make_pair(word, 1));
                    isinput[word] = true;
                }
                else
                {
                    if (isinput[word] == false)
                    {
                        word_allnumber_it->second++;
                        isinput[word] = true;
                    }
                }
                // 重置字母长度
                word_num = 0;
                // 重置单词缓存
                memset(word, 0, sizeof(word));
            }
        }
    }
    // 生成每个单词再文章中w的算法
    for (auto &s_and_w_it : _stringinfo_and_wordinfo)
    {
        for (auto &wordinfo_it : s_and_w_it.second.wordinfo)
        {
            auto word_allnumber_it = word_allnumber.find(wordinfo_it.first);
            if (word_allnumber_it != word_allnumber.end())
            {
                double n = _stringinfo_and_wordinfo.size();
                double df = word_allnumber_it->second;
                if (df > n - 1)
                {
                    df = n - 1;
                }
                double idf = log2(n / (df + 1));
                double w = idf * wordinfo_it.second.first;
                wordinfo_it.second.second = w;
                s_and_w_it.second.all_w += (w * w);
            }
        }
    }
    // 生成权重的算法，并且将权重存入“ _invertIndexTable”数据结构中
    for (auto &s_and_w_it : _stringinfo_and_wordinfo)
    {

        for (auto &wordinfo_it : s_and_w_it.second.wordinfo)
        {
            double ww = wordinfo_it.second.second / sqrt(s_and_w_it.second.all_w);
            _invertIndexTable[wordinfo_it.first].insert(make_pair(s_and_w_it.first, ww));
        }
    }
}

// ：生成倒排索引库文件
void PageLibPreprocessor::GetInvertIndexTableFile(const string filename)
{
    // 以写的方式发开filename路径所指的文件，并且判断文件是否可以打开
    ofstream invertfile;
    invertfile.open(filename, ios::out | ios::trunc);
    if (!invertfile.is_open())
    {
        perror("bGetInvertIndexTableFile:: invertfile.open()");
        return;
    }
    // 读取数据结构"_invertIndexTable"中的信息，并且将这些信息写入上方打开的文件中
    for (auto &it : _invertIndexTable)
    {

        invertfile << it.first;
        invertfile << "     ";
        for (auto &tt : it.second)
        {

            invertfile << tt.first;
            invertfile << " ";
            invertfile << tt.second;
            invertfile << "    ";
        }
        invertfile << endl;
    }
    // 关闭打开的文件，释放文件对象占用的资源
    invertfile.close();
}

// ：生成偏移量文件
void PageLibPreprocessor::GetOffSetLibFile(const string filename)
{
      // 以写的方式发开filename路径所指的文件，并且判断文件是否可以打开
    ofstream off_set_lib_file;
    off_set_lib_file.open(filename, ios::out | ios::trunc);
    if (!off_set_lib_file.is_open())
    {
        perror("GetOffSetLib:: off_set_lib_file.is_open()");
        return;
    }
        // 读取数据结构" _offsetLib"中的信息，并且将这些信息写入上方打开的文件中
    for (auto &it : _offsetLib)
    {

        off_set_lib_file << it.first;
        off_set_lib_file << " ";
        off_set_lib_file << it.second.first;
        off_set_lib_file << " ";
        off_set_lib_file << it.second.second;
        off_set_lib_file << endl;
    }
     // 关闭打开的文件，释放文件对象占用的资源
    off_set_lib_file.close();
}

// ：生成网页库文件
void PageLibPreprocessor::GetWebPageLibFile(const string filename)
{ 
     // 以写的方式发开filename路径所指的文件，并且判断文件是否可以打开
    ofstream web_page_file;
    web_page_file.open(filename, ios::out | ios::trunc);
    if (!web_page_file.is_open())
    {
        perror("GetWebPageLibFile:: web_page_file.is_open()");
        return;
    }
            // 读取数据结构" v_webpage"中的信息，并且将这些信息写入上方打开的文件中
    for (auto &it : v_webpage)
    {
        web_page_file << "<doc>";
        web_page_file << "<docid>";
        web_page_file << it._docId;
        web_page_file << "</docid>";
        web_page_file << endl;
        web_page_file << "<url>";
        web_page_file << it._docUrl;
        web_page_file << "</url>";
        web_page_file << endl;
        web_page_file << "<title>";
        web_page_file << it._docTitle;
        web_page_file << "</title>";
        web_page_file << endl;
        web_page_file << "<content>";
        web_page_file << it._docContent;
        web_page_file << "</content>";
        web_page_file << endl;
        web_page_file << "</doc>";
        web_page_file << endl;
        web_page_file << endl;
    }
    // 关闭打开的文件，释放文件对象占用的资源
    web_page_file.close();
}

// ：用于生成偏移量信息
// 现阶段只是传入_file,后期使用接口获取vector<string>，对接。
void PageLibPreprocessor::readInfoFormFiles()
{
    int position = 0;
    for (const auto &str : _file)
    {
        std::size_t start = str.find("<docid>") + 7;//找到含有dcid的标签头，提取标签id
        std::size_t end = str.find("</docid>");
        std::string intStr = str.substr(start, end - start);//提取标签id
        int id = std::stoi(intStr);
        int len = str.size();
        _offsetLib.insert({id, std::make_pair(position, len)});//插入——offsetlib中
        position += len;
    }
}

// ：用于生成偏移量、倒排库、网页库文件
void PageLibPreprocessor::storeOnDisk()
{
    //将三个文件的文件地址传入，并且调用生成三个文件的相关函数（倒排索引库、偏移库、网页库）
    string invert_index_filename = INVERTNAME;
    string off_set_lib_filename = OFFSETNAME;
    string web_page_lib_filename = WEBPAGENAME;
    GetInvertIndexTableFile(invert_index_filename);
    GetOffSetLibFile(off_set_lib_filename);
    GetWebPageLibFile(web_page_lib_filename);
}

// ：构造函数
PageLibPreprocessor::PageLibPreprocessor(list<WebPage>& pages, Configuration_jieba &conf)
: _cppJieba(conf)
, l_webpage(pages) 
{
    //：将网页库文章内容去重
    cutRedundantPages();
    //：将去重的内容写入到_file内
    vWebToString();
    //：生成文章偏移库
    readInfoFormFiles();
    //：生成倒排索引库
    buildInvertIndexTable();
}

//从v_webpage中提取网页信息生成list<string> _file用于偏移库生成
void PageLibPreprocessor::vWebToString() {
    for (auto iter = v_webpage.begin(); iter != v_webpage.end(); ++iter) {

        std::stringstream ss;
        ss << "<doc><docid>" << iter->_docId << "</docid><url>" << iter->_docUrl
            << "</url><title>" << iter->_docTitle << "</title><content>"
            << iter->_docContent << "</content></doc>";

        string fmtTxt = ss.str();
        _file.push_back(fmtTxt);
    }
}
