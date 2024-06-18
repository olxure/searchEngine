#include "../include/Configuration.h"
#include "../include/SplitToolCppJieba.h"
#include "../include/ChiDictProducer.h"

int main()
{
    //初始版，单独的Cppjieba
    vector<string> words;

    //读取配置文件
    Configuration conf1("../conf/Jieba.conf");
    //初始化Cppjieba
    SplitToolCppJieba cppJieba(conf1);

    Configuration conf2("../conf/chi.conf");
    ChiDictProducer dict(conf2, cppJieba);

    //创建词典
    dict.createDict();
    //将词典保存到文件
    dict.storeDict("chi_dict.dat");
    //将索引保存到文件
    dict.storeIndex("chi_index.dat");
    return 0;
}


