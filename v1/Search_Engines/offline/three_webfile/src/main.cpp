#include "../include/PageLibPreprocessor.h"
#include "../include/Configuration.h"
#include "../include/DirScanner.h"
#include "../include/FileProcessor.h"
#include "../include/PageLib.h"
#include "../include/WebPage.h"
#include <string>

using std::string;
int main()
{
    //调用函数创建唯一实例，路径传参并跳转到构造函数，在Configuration.h头文件中
    Configuration &config = Configuration::getInstance("../conf/config.txt");
    // 获取配置文件内容,加引用&保证全局唯一实例instance

    //获取dirname和../article并存储在configMap，注意引用，因此&configMap和_configMap是同一个
    const std::map<std::string, std::string> &configMap = config.getConfigMap();

    //打印测试文件，看看储存配置文件的键值对是否正确
    // for (const auto &pair : configMap)
    // {
    //     std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    // }

    //FileProcessor套PageLib，PageLib再套DirScanner
    DirScanner testdir;//DirScanner 类用于扫描指定目录中的所有文件，并将这些文件的路径存储在一个向量中
    //DirScanner testdir会调用 默认构造函数，进行初始化，不是在testdir()中初始化，但是完全初始化是在testdir()中
    //因为，testdir()中才会把private类中的数据成员_files初始化

    testdir();//// 扫描目录并填充文件名向量，vector<string> _files，所有路径存到_files中
    //调用 testdir(); 实际上是在调用 DirScanner 类的 operator() 函数
    //operator() 函数的返回类型是 void，因此testdir()没有返回类型

    PageLib pageLib(testdir);// 使用 DirScanner 对象初始化 PageLib 对象
    //PageLib 类负责从 DirScanner 类获取文件路径，并使用这些文件路径生成网页对象

    // 使用 PageLib 对象初始化 FileProcessor 对象
    FileProcessor fileProcessor(pageLib);//FileProcessor 类负责处理 PageLib 中的网页对象，并执行创建操作
    fileProcessor.create();

    string conf_string = "../conf/Jieba.conf";
    Configuration_jieba conf(conf_string);
    conf.readConfig();

    PageLibPreprocessor pp(pageLib.getWebPage(), conf);

    pp.storeOnDisk();

    return 0;
}
