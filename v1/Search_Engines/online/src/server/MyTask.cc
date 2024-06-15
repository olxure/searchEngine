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
    cout << "rece name: " << name << endl;
    cout << "recv passwd: " << passwd << endl;
    // 计算盐值
    srand((unsigned)time(NULL));
    char* salt;
    salt = genRandomString(SALT_LENGTH);

    cout << salt << endl;
    MYSQL* mysql =  mysql_init(NULL);
    MYSQL* Mret = mysql_real_connect(mysql, "localhost", "root", "mysql123", "client"
                                     , 0, NULL, 0);     // 连接数据库
    if(Mret == NULL){
        fprintf(stderr, "%s\n", mysql_error(mysql));
    }

    int ret;
    // 连接上数据库后开始执行插入语句
    char* crypted;
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
    string passwd(row[3]);
    char* crypted = crypt(password.c_str(), salt.c_str());
    if(passwd == string(crypted))
    {
        cout << "account has found!" << endl;
        LogInfo("user: %s login!", name.c_str());
        return 1;
    }
    else
    {
        return -2;
    }
}

char* MyTask::genRandomString(int length)       // 得到随机盐值
{
    int flag, i;
    char* string;
    if ((string = (char*) malloc(length)) == NULL )
    {
        printf("Malloc failed!flag:14\n");
        return NULL ;
    }
    for (i = 0; i < length+1; i++)
    {
        flag = rand() % 3;
        switch (flag)
        {
        case 0:
            string[i] = 'A' + rand() % 26;
            break;
        case 1:
            string[i] = 'a' + rand() % 26;
        	break;
        case 2:
            string[i] = '0' + rand() % 10;
            break;
        default:
            string[i] = 'x';
            break;
        }
    }
    string[length] = '\0';
    return string;
}

json MyTask::webSearch(Configuration* conf, string& name, string& usrMsg) {
    json j;
    MyRedis* redis = MyRedis::getInstance(conf);
    string redisResult = redis->get(usrMsg);
    cout << "Redis result: " << redisResult << endl;  // 添加调试信息

    if (redisResult.empty()) {
        // 缓存中没找到就走库搜索
        WebPageQuery WebQuery(conf);

        SplitToolCppJieba* pSplit = SplitToolCppJieba::getInstance(conf);

        // 用户的消息进行分词
        WebPageSearcher wps(usrMsg, *pSplit);
        vector<string> wordList = wps.getWordList();

        WebQuery.doQuery(wordList);

        WebQuery.handleAbstract();

        cout << "================" << endl;
        WebQuery.generateJson();
        j = WebQuery.getResult();
        cout << "Query result: " << j.dump() << endl;  // 添加调试信息

        // 把记录存储到redis中
        redis->set(usrMsg, j.dump());
    } else {
        // 找到直接返回
        try {
            j = json::parse(redisResult);
        } catch (const nlohmann::json::parse_error& e) {
            cout << "JSON parse error: " << e.what() << endl;
            // 可以根据需要处理错误，例如返回空的 JSON 对象或退出程序
            return json::object();
        }
    }
    pushHistory(name, usrMsg);
    return j;
}



//有点问题暂存
// json MyTask::webSearch(Configuration* conf, string& name, string& usrMsg)
// {
//     // 网页搜索
//     json j;
//     MyRedis* redis = MyRedis::getInstance(conf);
//     string redisResult = redis->get(usrMsg);
//     if(redisResult == string())
//     {
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
//         // 把记录存储到redis中
//         redis->set(usrMsg, j.dump());
//     }
//     else
//     {
//         // 找到直接返回
//         j = json::parse(redisResult);    
//     }
//     pushHistory(name, usrMsg);
//     return j;
// }

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
