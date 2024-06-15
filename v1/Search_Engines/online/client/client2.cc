#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <limits>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
//#include "json.hpp"
#include <nlohmann/json.hpp>
#define COLOR_CYAN    "\x1b[36m"  // 青蓝色
#define COLOR_RESET   "\x1b[0m"   // 默认颜色
#define COLOR_YELLOW  "\x1b[33m"  // 黄色

using namespace std;

typedef struct train_s {//传输文件协议（小火车）
    int len;
    int id;
    std::string data;
} train;

typedef struct train_t{
    int len;
    std::string data;
}train_t;

void recvn(int sockfd, char buf[], int len)
{
    int left = len;
    int ret = 0;
    while(left > 0)
    {
        ret = recv(sockfd, buf, left, 0);
        left -= ret;
        buf += ret;
    }
}

void sendTrainNamePasswd(int sockfd, int id, string name, string passwd)//发送用户名和密码
{
    train_s train;
    train.data = name+passwd;
    train.len = name.size();
    train.id = id;
    
    size_t buffer_size = sizeof(train.len) + sizeof(train.id) + train.data.length();
    
    char* buffer = new char[buffer_size + 1]();
    std::memcpy(buffer, &train.len, sizeof(train.len));
    std::memcpy(buffer + sizeof(train.len), &train.id, sizeof(train.id));
    std::memcpy(buffer + sizeof(train.len) + sizeof(train.id), train.data.c_str(), train.data.length());

    // 服务器是接收到换行才会停止
    buffer[buffer_size] = '\n';

    if (send(sockfd, buffer, buffer_size + 1, 0) < 0) {
        std::cout << "-------Error: Send failed!-------" << std::endl;
    }
    delete [] buffer;
}

void receiveTrainLogin(int sockfd, train_t& recvTrain)//接收服务端返回信息
{
    if(recv(sockfd, &recvTrain.len, sizeof(int), 0) < 0)
    {
        cout << "-------Error: Receive failed!-------" << endl;
    }
    int left = recvTrain.len;
    int data;
    if(recv(sockfd, &data, left, 0) < 0)
    {
        cout << "-------Error: Receive failed!-------" << endl;
    }
    recvTrain.data.append(reinterpret_cast<char*>(&data), sizeof(data));
}

void getNameAndPasswd(std::string& name, std::string& passwd)//用户输入用户名和密码
{
    std::cout<<"Please enter username:";
    std::cin >> name;

    std::cout<<"Please enter password:";
    std::cin >> passwd;
}


// 登入
string hasAccount(int sockfd)
{
    // 循环执行下面语句
    int cnt = 3;        // 密码输错的最多次数

    while(cnt > 0)
    {
        //先发送1给服务器
        int dataId = 3;
        //输入用户名和密码
        string name, passwd;
        getNameAndPasswd(name, passwd);
        
        //小火车发送用户名和密码
        sendTrainNamePasswd(sockfd, dataId, name, passwd);
        
        //成功服务器返回0，用户名错误返回1，密码错误返回2
        train_t result;
        receiveTrainLogin(sockfd, result);
        int ret = *(int*)(result.data.data());
        if(ret == 1)
        {
            return name;
        }
        else if(ret == -1)
        {
            cout << "用户名输入错误，请重新输入" << endl;
        }
        else
        {
            cout << "密码输入错误，请重新输入" << endl;
            --cnt;
        }
    }

    cout << "密码错误次数过多" << endl;
    return string();
}

// 注册
string signUp(int sockfd)
{
    // 先发送2给服务器
    int dataId = 4;
    
    // 输入用户名和密码
    string name, passwd;
    getNameAndPasswd(name, passwd);

    cout << name << endl;
    cout << passwd << endl;
    // 小火车发送用户名和密码
    sendTrainNamePasswd(sockfd, dataId, name, passwd);
    
    return name;
}

void outRedcolor(const std::string& sum, const std::string& usrIn);
int printVersion(void);
void runSearch(size_t sockfd, string& name);

void printWithGradientColor(const std::string& text, int depth) {//输出红色字符
    int colorCode = 31 + (depth % 7);  // 31-37为红至白色的颜色代码
    std::cout <<std::setfill('-')<<std::setw(50)<<"-";
    std::cout <<std::setfill(' ')<<std::setw(5)<<"\033[" << colorCode << "m" << text << "\033[0m"<<std::setw(5)<<"";
    std::cout <<std::setfill('-')<<std::setw(50)<<"-"<<std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(40));

}

int main(int argc, char* argv[]) {
    // 检查输入编译信息
    if (argc != 3) {
        std::cout << "-------Error: Incorrect number of arguments!-------" << std::endl;
        return -1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);//初始化套接字
    
    sockaddr_in serAdd{};
    serAdd.sin_family = AF_INET;
    serAdd.sin_port = htons(std::stoi(argv[2]));
    serAdd.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr*)&serAdd, sizeof(serAdd)) != 0) {//建立链接
        std::cout << "-------Error: Link failed!-------" << std::endl;
        return -1;
    }

    printVersion();//输出封面效果
    //显示热词
    
    int len = 0;
    recv(sockfd, &len, sizeof(len), 0);//接收热词信息
    char buf[len];
    memset(buf, 0, sizeof(buf));
    recv(sockfd, buf, len, 0);
    std::string Jsonstring(buf, len);
    nlohmann::json jsonData = nlohmann::json::parse(Jsonstring);
    int depth = 0;
    std::cout <<std::setfill('-')<<std::setw(46)<<"-";
    std::cout <<std::setfill(' ')<<std::setw(5)<<"\033[" << COLOR_CYAN  << "<HOT WORD LIST>" << "\033[0m"<<std::setw(5)<<"";
    std::cout <<std::setfill('-')<<std::setw(46)<<"-"<<std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    for(const auto& val:jsonData){
        std::string stringValue = val;
        printWithGradientColor(stringValue,depth);//输出热词
        ++depth;
    }

    while(true){//进入循环搜索逻辑
        std::cout<<"If you have an account, please enter (1).No account,create an account Please enter (2). Visiter mode Please enter (3).Quit Please enter (0):";
        int account;
        std::cin>>account;
        if(account==1){//用户有账户
            
            string name = hasAccount(sockfd);     // 登入操作

            if(name == string())
            {
                return -1;
            }
            else{
                cout << "login success!" << endl;
            }

            //查看历史记录
            int his;
            std::cout<<"View History Please enter 0: ";
            std::cin>>his;
            if(his==0){//输出用户的数据库
                int histo=5;
                sendTrainNamePasswd(sockfd, histo, name, string());

                int len=0;
                recv(sockfd,&len,sizeof(len),0);
                char buf[len];
                memset(buf,0,sizeof(buf));
                recv(sockfd,buf,len,0);//接收历史记录

                std::cout << endl <<"-------View History-------"<<std::endl;
                std::string jsonstring(buf, len);
                nlohmann::json jsonData = nlohmann::json::parse(jsonstring);//解析为json格式
                for(const auto& val:jsonData){//输出历史记录
                    std::string stringValue = val;
                    std::cout<<stringValue<<std::endl;
                }
                cout << endl;
            }

            runSearch(sockfd, name);

        }else if(account==2){//用户创建新账户

            string name = signUp(sockfd);     // 注册操作

            std::cout<<"Account created successfully!"<<std::endl;
            //登录成功
            runSearch(sockfd, name);
        }else if(account==3){//游客模式访问
            std::cout<<"-------Visiter mode-------"<<std::endl;
            string name;
            runSearch(sockfd, name);
        }
        else if(account==0)
        {
            system("clear");
            sleep(8);
            system("ascii-image-converter -C ./pic.png");
            exit(1);
        }
        else{
            std::cout << "-------Error: Please enter a number!-------" << std::endl;
            continue;
        }
    }

    close(sockfd);
    return 0;
}

void runSearch(size_t sockfd, string& name) {
    while (true) {
        // 获取搜索方式
        std::cout << "Please select the method you want to search: [keyword(1)][webpage(2)][quit(0)]:";
        int choice;
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cout << "-------Error: Please enter a number!-------" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (choice == 1) { // keyword
            // 获取输入信息
            std::string line;
            std::cout << "Please enter your search: ";
            std::cin.ignore();  // 忽略之前的换行符
            std::getline(std::cin, line);
            if (std::cin.fail()) {
                std::cout << "-------Error: Please enter a string!-------" << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            train t1;
            t1.id = 1;
            t1.data = line;
            t1.len = line.length();

            // 转换为字节流
            using std::cout;
            using std::endl;
            size_t buffer_size = sizeof(t1.len) + sizeof(t1.id) + t1.data.length();
            cout << "buffer_size: " << buffer_size << endl;  // 添加调试信息

            char* buffer = new char[buffer_size + 1]();
            std::memcpy(buffer, &t1.len, sizeof(t1.len));
            std::memcpy(buffer + sizeof(t1.len), &t1.id, sizeof(t1.id));
            std::memcpy(buffer + sizeof(t1.len) + sizeof(t1.id), t1.data.c_str(), t1.data.length());

            buffer[buffer_size] = '\n';

            // 发送字节流
            if (send(sockfd, buffer, buffer_size + 1, 0) < 0) {
                std::cout << "-------Error: Send failed!-------" << std::endl;
                continue;
            }

            delete[] buffer;

            int len = 0;
            recv(sockfd, &len, sizeof(len), 0);
            char buf[len];
            memset(buf, 0, sizeof(buf));
            recv(sockfd, buf, len, 0);
            std::string Jsonstring(buf, len);

            cout << "jsonstring: " << Jsonstring << endl;  // 添加调试信息
            try {
                nlohmann::json jsonData = nlohmann::json::parse(Jsonstring);
                cout << endl;
                for (const auto& val : jsonData) {
                    std::string stringValue = val;
                    std::cout << stringValue << std::endl;
                }
                cout << endl;
            } catch (const nlohmann::json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
            }
        }
        // 其他选择的代码省略
    }
}

int printVersion(void) {
    std::cout << "\033[2J\033[1;1H";

    printf("%s  _                   \n", COLOR_CYAN);
    printf(" | |                  \n");
    printf(" | |      _____  __  \n");
    printf(" | |     / _ \\ \\/ / \n");
    printf(" | |____|  __/>  <   \n");
    printf(" |______|\\___/_/\\_\\ \n");
    printf(" version 1.0.0%s\n", COLOR_YELLOW);
    return 0;
}

void outRedcolor(const std::string& sum, const std::string& usrIn) {
    for (char ch : sum) {
        if (usrIn.find(ch) != std::string::npos) {
            std::cout << "\033[31m" << ch << "\033[0m";  // 设置输出颜色为红色
        } else {
            std::cout << ch;
        }
    }
    std::cout<<std::endl;
}

