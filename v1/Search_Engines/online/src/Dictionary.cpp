#include "../include/Dictionary.h"

Dictionary* Dictionary::getInstance(Configuration* conf){
    if(m_dictionary==nullptr){
        std::unique_lock<std::mutex> lock(m_Mutex);//实现单例类线程安全
        if(m_dictionary==nullptr){
            auto temp = new (std::nothrow)Dictionary();
            m_dictionary = temp;
            map<string, string> confMap = conf->getConfigMap();

            for(auto& elem : confMap)//使用配置文件初始化数据
            {
                if(elem.first == "chi_dict.dat")
                {
                    m_dictionary->initDictCh(elem.second);
                }
                else if(elem.first == "chi_index.dat")
                {
                    m_dictionary->initIndexCh(elem.second);
                }
                else if(elem.first == "eng_dict.dat")
                {
                    m_dictionary->initDictEn(elem.second);
                }
                else if(elem.first == "eng_index.dat")
                {
                    m_dictionary->initIndexEn(elem.second);
                }
            }
            /* m_dictionary->initDictCh("/home/lihaha/wd_project/Search_Engines/online/data/chi_dict.dat"); */
            /* m_dictionary->initIndexEn("/home/lihaha/wd_project/Search_Engines/online/data/eng_index.dat"); */
            /* m_dictionary->initDictEn("/home/lihaha/wd_project/Search_Engines/online/data/eng_dict.dat"); */
            /* m_dictionary->initIndexCh("/home/lihaha/wd_project/Search_Engines/online/data/chi_index.dat"); */

        }
    }
    return m_dictionary;
}
void Dictionary::deleteInstance(){
    std::unique_lock<std::mutex> lock(m_Mutex);
    if(m_dictionary){
        delete m_dictionary;
        m_dictionary=nullptr;
    }
}
Dictionary *Dictionary::m_dictionary = nullptr;
std::mutex Dictionary::m_Mutex;


void Dictionary::initDictCh(const string &dictPathCh){
    ifstream ifs(dictPathCh);
    if(!ifs){
        cerr<<"can not open file dictPathCh"<<endl;
        return;
    }
    string line;
    while(getline(ifs,line)){
        istringstream iss(line);
        string key;
        int value;
        if(iss>>key>>value){
            _dictCh.push_back(make_pair(key,value));
        }
        else{
        cerr<<"can not analyze _dictCh line:"<<key<<endl;
        }
    }
}
void Dictionary::initDictEn(const string &dictPathEn){
    ifstream ifs(dictPathEn);
    if(!ifs){
        cerr<<"can not open file dictPathEn"<<endl;
        return;
    }
    string line;
    while(getline(ifs,line)){
        istringstream iss(line);
        string key;
        int value;
        if(iss>>key>>value){
            _dictEn.push_back(make_pair(key,value));
        }
        else{
        cerr<<"can not analyze _dictEn line:"<<key<<endl;
        }
    }
}

void Dictionary::initIndexCh(const string &indexPathCh){
    ifstream ifs(indexPathCh);
    if(!ifs){
        cerr<<"can not open indexPathCh"<<endl;
        return;
    }
    string line;
    while(getline(ifs,line)){
        string key;
        istringstream iss(line);
        if(iss>>key){
            int value;
            while(iss>>value){
                _indexCh[key].insert(value);
            }
        }
    }
}
void Dictionary::initIndexEn(const string &indexPathEn){
    ifstream ifs(indexPathEn);
    if(!ifs){
        cerr<<"can not open indexPathEn"<<endl;
        return;
    }
    string line;
    while(getline(ifs,line)){
        string key;
        istringstream iss(line);
        if(iss>>key){
            int value;
            while(iss>>value){
                _indexEn[key].insert(value);
            }
        }
    }
}

vector<pair<string,int>>& Dictionary::getDictCh(){return _dictCh;}
vector<pair<string,int>>& Dictionary::getDictEn(){return _dictEn;}

map<string,set<int>>& Dictionary::getIndexCh(){return _indexCh;}
map<string,set<int>>& Dictionary::getIndexEn(){return _indexEn;}
//vector<pair<string,int>>& Dictionary::getFinRes(){return _RecWord;} 

set<int> Dictionary::findWord(const string& ch,map<string,set<int>> _index){
    set<int> lineNumbers;

    auto iter = _index.find(ch);
    if(iter != _index.end())
    {
        lineNumbers = iter->second;
    }

    return lineNumbers;
}

//识别词语中的汉字，并把每个字存入wordset中
void Dictionary::CutWord(const string words, vector<string> &wordset)
{
  for (size_t i = 0; i < words.size();)
  {
    int cplen = 1;
    if ((words[i] & 0xf8) == 0xf0)
      cplen = 4;
    else if ((words[i] & 0xf0) == 0xe0)
      cplen = 3;
    else if ((words[i] & 0xe0) == 0xc0)
      cplen = 2;
    else
        cplen = 1;
    if ((i + cplen) > words.length())
      cplen = 1;
    wordset.push_back(words.substr(i, cplen));
    i += cplen;
  }
  return ;
}

vector<pair<string,int>>& Dictionary::doAndQuery(const string & str){
//void Dictionary::doAndQuery(const string & str){
    set<int> finResCh;
    set<int> finResEn;
    bool isch =false;
    bool isen =false;

    vector<string> wordSet;
    CutWord(str, wordSet);
    set<int> memSet;

    for(auto& elem : wordSet){
        cout << "elem: " << elem << endl;
        if(isChinese(elem)){
            isch=true;
            set<int> finIt = findWord(elem,_indexCh);
            finResCh.insert(finIt.begin(), finIt.end());

        }else if(isEnglish(elem)){
            isen = true;
            auto it = _indexEn.find(elem);
            // cout << endl;
            set<int> memSet=it->second;
            finResEn.insert(memSet.begin(),memSet.end());
        }else{
            continue;
        }
    }
    _RecWord.clear();    
    if(isch==true){
        for(auto elem:finResCh){
            _RecWord.push_back(make_pair(_dictCh[elem-1].first,_dictCh[elem-1].second));
        }
    }
    if(isen==true){
        for(auto elem:finResEn){
            _RecWord.push_back(make_pair(_dictEn[elem - 1].first,_dictEn[elem-1].second));
        }
    }
    
    return _RecWord;
}
    
bool Dictionary::isChinese(const string& word){
    if ((word[0] & 0xf8) == 0xf0)
      return true;
    else if ((word[0] & 0xf0) == 0xe0)
      return true;
    else if ((word[0] & 0xe0) == 0xc0)
      return true;

    return false;
}

bool Dictionary::isEnglish(const string& word){
    char ch = word[0];
    if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
        return true;
    }
    return false;
}


