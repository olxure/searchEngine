#ifndef OLX_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_DIRSCANNER_H_
#define OLX_PROJECT_SEARCH_ENGINES_OFFLINE_THREE_WEBFILE_INCLUDE_DIRSCANNER_H_

// Consider using <filesystem> for cross-platform compatibility (C++17)
#include "../include/Configuration.h"

#include <vector>
#include <string>
#include <dirent.h>
#include <unistd.h>

using std::string;
using std::vector;

class DirScanner {
public:
    DirScanner() {}
    //第一个括号重载的是函数调用操作符 ()，第二个括号 () 是函数参数列表，这里参数为空而已，表示重载函数不接受任何参数
    void operator()() {
        //单例模式中的参数是用于在首次实例化对象时进行初始化的。如果该参数在首次调用 getInstance 时没有被使用，那么传递空字符串没有什么实际意义
        Configuration& config = Configuration::getInstance("");  // getInstance是一个静态方法，这里第二次调用，传参没有任何影响
        map<string, string>& configMap = config.getConfigMap();//getConfigMap()返回的是，return _configMap;

        // Add error handling here

        string key_dir = "dirname";//这里赋值dirname与config.txt中的键的dirname保持一致
        string dirname = configMap[key_dir];//通过map下标的dirname来找到对应的../article

        //通过traverse这个递归函数遍历指定目录../article及其子目录中的所有文件
        //并将遍历到的../article文件所在的路径存储在 _files 成员变量中。这个函数没有返回值，而是通过修改类的私有成员变量 _files 来实现其功能。
        traverse(dirname);//这里string类的dirname的赋值为../article
        //
    }

    vector<string>& getFiles() {
        return _files;
    }

private:
    // Traverse all files under the specified directory (dirname) and store their absolute paths in _files
    //dirname从配置文件config中中获取，dirname存储目录的路径，即../article
    void traverse(const string& dirname) {
        //opendir是POSIX 标准库函数，用于打开一个目录，并返回一个指向 DIR 结构的指针
        //失败时：返回 NULL，并设置 errno 以指示错误原因
        DIR* dir = opendir(dirname.c_str());//dirname.c_str()是将string类的../article转化成为c风格的字符串，这里dirname已经是../article初始化了
        //opendir是c标准库函数，因此要把string类改成c_str风格
        // Error handling
        if (dir == nullptr) {
            std::cerr << "Failed to open directory: " << dirname << std::endl;
            return;
        }
        //上面的opendir打开了目录../article的话，接下来就是readdir读目录中的文件
        dirent* entry;//定义一个目录结构，在下面的while中初始化
        while ((entry = readdir(dir)) != nullptr) {//readdir是c标准库函数，不是POSIX
            //每次调用 readdir，它都会返回一个指向表示目录项的 struct dirent 的指针，直到目录中的所有条目都被读取完毕，此时返回 nullptr
            //因此，readdir返回空指针被entry获取的时候，循环结束

            //entry->d_name 是当前读取到的目录项的名称。它可能是一个文件名、子目录名，或者是特殊的目录项名（例如当前目录.和父目录..）
            //可以理解为ll命令下，一个目录下所有存在的东西，目录项不单单只包括文件
            //entry->d_name 是一个指向字符数组（C 风格字符串）的指针
            //std::string 类有一个构造函数，可以接受 const char* 参数，并创建一个 std::string 对象。这个过程不是强制转换，而是构造函数的正常调用。
            std::string fileName = entry->d_name;//使用构造函数类型为std::string(const char* s);用等号和用括号大部分情况是一样的
            //const char* cstr = "example.txt";
            //std::string fileName1 = cstr;//拷贝初始化，用等号
            //std::string fileName2(cstr);//直接初始化，用括号

            // Ignore current directory and parent directory
            if (fileName == "." || fileName == "..") {
                continue;
            }

            std::string filePath = dirname + "/" + fileName;//../article下面的文件的名字，如../article/auto.xml
            if (entry->d_type == DT_DIR) {
                // Recursively traverse subdirectories
                traverse(filePath);//递归中发现一个目录下还有一个目录，那么再递归
            } else if (entry->d_type == DT_REG) {//DT_DIR: 目录  DT_REG: 常规文件
                // Store the file path in the vector
                _files.push_back(filePath);//vector<string> _files 在push之后filePath被初始化为../article/auto.xml
            }
        }

        closedir(dir);
    }//traverse函数体结束

//     for (const auto& file : _files) {
//     std::cout << file << std::endl;
// }//可以验证pushback的路径对不对，存储多个路径的时候
//_files = { "../article/auto.xml", "../article/test.xml" };




private:
    vector<string> _files;//只定义，在traverse里面初始化
};

#endif

