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

    Configuration &config = Configuration::getInstance("../conf/config.txt");
    // 获取配置文件内容
    const std::map<std::string, std::string> &configMap = config.getConfigMap();

    // for (const auto &pair : configMap)
    // {
    //     std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    // }
    DirScanner testdir;
    testdir();
    PageLib pageLib(testdir);
    FileProcessor fileProcessor(pageLib);
    fileProcessor.create();

    string conf_string = "../conf/Jieba.conf";
    Configuration_jieba conf(conf_string);
    conf.readConfig();

    PageLibPreprocessor pp(pageLib.getWebPage(), conf);

    pp.storeOnDisk();

    return 0;
}
