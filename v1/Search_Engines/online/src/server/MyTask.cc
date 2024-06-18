#include "MyTask.h"
#include <shadow.h>
#include <string>
#include <string.h>
#include <crypt.h>
#include <memory>
#include <mysql/mysql.h>

#define SALT_LENGTH 10      // 盐值的长度
using std::shared_ptr;

void MyTask::signUp(string name, string passwd)
{
    cout << "recv name: " << name << endl;
    cout << "recv passwd: " << passwd << endl;
    // 
    srand((unsigned)time(NULL));// 生成盐值
    char* salt;//暂存
    salt = genRandomString(SALT_LENGTH);

    cout << "Generated salt: " << salt << endl; // 增加日志输出，测试盐值是否正确
    //cout << salt << endl;//暂存
    MYSQL* mysql =  mysql_init(NULL);
    MYSQL* Mret = mysql_real_connect(mysql, "localhost", "root", "mysql123", "client", 0, NULL, 0);     // 连接数据库
    if(Mret == NULL){
        fprintf(stderr, "%s\n", mysql_error(mysql));
    }

    int ret;
    // 连接上数据库后开始执行插入语句
    //char* crypted;//暂存
    char* crypted = crypt(passwd.c_str(), salt);    // 加密密码，测试用
    cout << "Encrypted password: " << crypted << endl; // 增加日志输出，测试用

    char sql[4096];
    bzero(sql, sizeof(sql));
    crypted = crypt(passwd.c_str(), salt);    // 加密密码
    sprintf(sql, "insert into info(user, salt, crypted) values('%s', '%s', '%s')", 
            name.c_str(), salt, crypted);    // 将用户名，盐值，以及加密后的密文存放到数据库中

    ret = mysql_query(mysql, sql);          // 执行插入语句
    if(ret != 0){
        fprintf(stderr, "%s\n", mysql_error(mysql));
    }

    mysql_close(mysql);
    free(salt);
    LogInfo("user: %s sign up!", name.c_str());
}

int MyTask::login(string name, string password)
{
    MYSQL* mysql = mysql_init(NULL);
    MYSQL* Mret = mysql_real_connect(mysql, "localhost", "root", "mysql123", "client",
                                     0, NULL, 0);
    if(Mret == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(mysql));
        return -1;
    }

    char sql[4096] = {0};
    sprintf(sql, "select * from info where user = '%s';", name.c_str());
    int ret = mysql_query(mysql, sql);
    if(ret != 0)        // 语法错误
    {    
        fprintf(stderr, "%s\n", mysql_error(mysql)); 
        return -3;      
    }
    MYSQL_RES* result = mysql_store_result(mysql);
    MYSQL_ROW row;
    if((row = mysql_fetch_row(result)) == NULL)
    {
        return -1;      // 没找到
    }
    string salt(row[2]);
    string passwd(row[3]);//暂存
    //string stored_passwd(row[3]);//测试
    cout << "Retrieved salt: " << salt << ", stored password: " << passwd << endl; // 增加日志输出，测试盐值用

    char* crypted = crypt(password.c_str(), salt.c_str());
    if(passwd == string(crypted))
    {
        cout << "account has found!" << endl;
        LogInfo("user: %s login!", name.c_str());
        return 1;
    }
    else
    {
        cout << "Password mismatch. Stored: " << passwd << ", Input: " << crypted << endl; // 增加日志输出，测试盐值
        return -2;
    }
}

char* MyTask::genRandomString(int length)       // 得到随机盐值
{
    int flag, i;
    char* string;
    // 动态分配内存来存储生成的随机字符串，如果分配失败则返回 NULL
    //if ((string = (char*) malloc(length)) == NULL )//暂存 // 分配长度为 length 的内存
    if ((string = (char*) malloc(length + 1)) == NULL )// 分配长度为 length + 1 的内存，包含终止符，测试用
    {
        printf("Malloc failed!flag:14\n");
        return NULL ;// 返回 NULL 表示分配失败
    }
    for (i = 0; i < length; i++)// 循环生成每个字符，测试
    //for (i = 0; i < length+1; i++)// 循环生成每个字符，暂存
    {
        flag = rand() % 3;// 生成 0 到 2 的随机数，决定字符类型
        switch (flag)
        {
        case 0:
            string[i] = 'A' + rand() % 26;// 生成大写字母
            break;
        case 1:
            string[i] = 'a' + rand() % 26;// 生成小写字母
        	break;
        case 2:
            string[i] = '0' + rand() % 10;// 生成数字
            break;
        default:
            string[i] = 'x';// 出现异常时生成字符 'x'
            break;
        }
    }//for循环结束
    string[length] = '\0';// 在字符串末尾添加空字符，表示字符串结束
    return string;// 返回生成的随机字符串
}//char* MyTask::genRandomString(int length)结束

// json MyTask::webSearch(Configuration* conf, string& name, string& usrMsg) {
//     json j;
//     MyRedis* redis = MyRedis::getInstance(conf);
//     string redisResult = redis->get(usrMsg);
//     cout << "Redis result: " << redisResult << endl;  // 添加调试信息

//     if (redisResult.empty()) {
//         // 缓存中没找到就走库搜索
//         WebPageQuery WebQuery(conf);

//         SplitToolCppJieba* pSplit = SplitToolCppJieba::getInstance(conf);

//         // 用户的消息进行分词
//         WebPageSearcher wps(usrMsg, *pSplit);
//         vector<string> wordList = wps.getWordList();

//         WebQuery.doQuery(wordList);

//         WebQuery.handleAbstract();

//         cout << "================" << endl;
//         WebQuery.generateJson();
//         j = WebQuery.getResult();
//         cout << "Query result: " << j.dump() << endl;  // 添加调试信息

//         // 把记录存储到redis中
//         redis->set(usrMsg, j.dump());
//     } else {
//         // 找到直接返回
//         try {
//             j = json::parse(redisResult);
//         } catch (const nlohmann::json::parse_error& e) {
//             cout << "JSON parse error: " << e.what() << endl;
//             // 可以根据需要处理错误，例如返回空的 JSON 对象或退出程序
//             return json::object();
//         }
//     }
//     pushHistory(name, usrMsg);
//     return j;
// }



//有点问题暂存
json MyTask::webSearch(Configuration* conf, string& name, string& usrMsg)
{
    // 网页搜索
    json j;// 用于存储搜索结果的 JSON 对象
    MyRedis* redis = MyRedis::getInstance(conf);// 获取 Redis 实例
    string redisResult = redis->get(usrMsg);// 从 Redis 缓存中获取用户消息的搜索结果
    if(redisResult == string()) // 如果缓存中没有找到结果
    {
        // 缓存中没找到就走库搜索
        WebPageQuery WebQuery(conf);// 创建 WebPageQuery 对象

        SplitToolCppJieba* pSplit = SplitToolCppJieba::getInstance(conf);// 获取分词工具实例

        // 用户的消息进行分词
        WebPageSearcher wps(usrMsg, *pSplit);// 创建 WebPageSearcher 对象，传入用户消息和分词工具
        vector<string> wordList = wps.getWordList();// 获取分词后的单词列表

        WebQuery.doQuery(wordList);// 执行查询

        WebQuery.handleAbstract();// 处理摘要

        cout << "================" << endl;
        WebQuery.generateJson();// 生成 JSON 结果
        j = WebQuery.getResult();// 获取 JSON 结果
        // 把记录存储到redis中
        redis->set(usrMsg, j.dump());
    }
    else// 如果缓存中找到结果
    {
        // 找到直接返回
        j = json::parse(redisResult);// 解析 JSON 结果    
    }
    pushHistory(name, usrMsg);// 记录搜索历史
    return j;// 返回 JSON 结果
}//json MyTask::webSearch(Configuration* conf, string& name, string& usrMsg)结束

string MyTask::getUserHistoryJSON(const string& str){
    std::string json;
    MYSQL *con;

    try {
        con = mysql_init(NULL);
        if (con == NULL) {
            std::cout << "Failed to initialize MySQL connection" << std::endl;
            return json;
        }

        con = mysql_real_connect(con, "127.0.0.1", "root", "mysql123", "client", 3306, NULL, 0);
        if (con == NULL) {
            std::cout << "Failed to connect to MySQL server: " << mysql_error(con) << std::endl;
            return json;
        }

        std::string query = "SELECT history FROM usrHistory WHERE name = '" + str + "'";
        if (mysql_query(con, query.c_str()) != 0) {
            std::cout << "5Failed to execute MySQL query: " << mysql_error(con) << std::endl;
            return json;
        }

        MYSQL_RES *res = mysql_use_result(con);
        if (res == NULL) {
            std::cout << "Failed to get MySQL result: " << mysql_error(con) << std::endl;
            return json;
        }

        MYSQL_ROW row;
        std::vector<std::string> historyArr;
        while ((row = mysql_fetch_row(res)) != NULL) {
            historyArr.push_back(row[0]);
        }

        json = "[";
        for (int i = 0; i < historyArr.size(); ++i) {
            if (i > 0) {
                json += ",";
            }
            json += "\"" + historyArr[i] + "\"";
        }
        json += "]";

        mysql_free_result(res);
        mysql_close(con);
    } catch (...) {
        std::cout << "An error occurred in getTopTenHistoryJSON()" << std::endl;
    }

    return json;
}


string MyTask::getTopTenHistoryJSON() {
    std::string json;
    MYSQL *con;

    try {
        con = mysql_init(NULL);
        if (con == NULL) {
            std::cout << "Failed to initialize MySQL connection" << std::endl;
            return json;
        }

        con = mysql_real_connect(con, "127.0.0.1", "root", "mysql123", "client", 3306, NULL, 0);
        if (con == NULL) {
            std::cout << "Failed to connect to MySQL server: " << mysql_error(con) << std::endl;
            return json;
        }

        std::string query = "SELECT history FROM countHistory ORDER BY count DESC LIMIT 10";
        if (mysql_query(con, query.c_str()) != 0) {
            std::cout << "5Failed to execute MySQL query: " << mysql_error(con) << std::endl;
            return json;
        }

        MYSQL_RES *res = mysql_use_result(con);
        if (res == NULL) {
            std::cout << "Failed to get MySQL result: " << mysql_error(con) << std::endl;
            return json;
        }

        MYSQL_ROW row;
        std::vector<std::string> historyArr;
        while ((row = mysql_fetch_row(res)) != NULL) {
            historyArr.push_back(row[0]);
        }

        json = "[";
        for (int i = 0; i < historyArr.size(); ++i) {
            if (i > 0) {
                json += ",";
            }
            json += "\"" + historyArr[i] + "\"";
        }
        json += "]";

        mysql_free_result(res);
        mysql_close(con);
    } catch (...) {
        std::cout << "An error occurred in getTopTenHistoryJSON()" << std::endl;
    }

    return json;
}


void MyTask::pushHistory(const std::string& usrName,const std::string& str) {
    MYSQL *con;
    try {
        con = mysql_init(NULL);
        if (con == NULL) {
            std::cout << "Failed to initialize MySQL connection" << std::endl;
            return;
        }

        con = mysql_real_connect(con, "127.0.0.1", "root", "mysql123", "client", 3306, NULL, 0);
        if (con == NULL) {
            std::cout << "Failed to connect to MySQL server: " << mysql_error(con) << std::endl;
            return;
        }

        std::string query = "INSERT IGNORE INTO usrHistory (name, history) VALUES ('" + usrName + "', '" + str + "')";
        if (mysql_query(con, query.c_str()) != 0) {
            std::cout << "1Failed to execute MySQL query: " << mysql_error(con) << std::endl;
            return;
        }

        query = "SELECT * FROM countHistory";
        if (mysql_query(con, query.c_str()) != 0) {
            std::cout << "2Failed to execute MySQL query: " << mysql_error(con) << std::endl;
            return;
        }

        MYSQL_RES *res = mysql_use_result(con);
        if (res == NULL) {
            std::cout << "Failed to get MySQL result: " << mysql_error(con) << std::endl;
            return;
        }

        bool strExists = false;
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != NULL) {
            std::string name = row[0];
            if (name == str) {
                strExists = true;

                int count = std::stoi(row[1]) + 1;
                query = "UPDATE countHistory SET count = " + std::to_string(count) + " WHERE history = '" + str + "'";
                mysql_free_result(res); // 释放上一个结果集
                if (mysql_query(con, query.c_str()) != 0) {
                    std::cout << "3Failed to execute MySQL query: " << mysql_error(con) << std::endl;
                    return;
                }

                break;
            }
        }

        //mysql_free_result(res);

        if (!strExists) {
            query = "INSERT INTO countHistory (history, count) VALUES ('" + str + "', 1)";
            if (mysql_query(con, query.c_str()) != 0) {
                std::cout << "4Failed to execute MySQL query: " << mysql_error(con) << std::endl;
                return;
            }
        }

        mysql_close(con);
    } catch (...) {
        std::cout << "An error occurred in pushHistory()" << std::endl;
    }
}
