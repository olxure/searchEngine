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
//定义两个结构体用于封装和解析数据，以便通过网络套接字进行传输，一个用于发送，一个用于接收
typedef struct train_s {//传输文件协议（小火车）
    int len;//数据长度
    int id;// 数据ID，用于标识不同类型的数据（例如用户名、密码等）
    std::string data;// 实际传输的数据
} train;
//这个结构体的作用是将要发送的数据封装成一个小火车（类似于数据包的概念），
//包含数据的长度、数据的类型ID和实际的数据内容。这样在传输时，可以明确知道传输的数据的长度和类型，方便在接收时解析。
//在发送用户名和密码时，将用户名和密码拼接成一个字符串，并封装到 train_s 结构体中，然后将整个结构体通过套接字发送

typedef struct train_t{
    int len;// 数据长度
    std::string data;// 实际传输的数据
}train_t;
//这个结构体用于接收服务器返回的数据。它包含数据的长度和实际的数据内容。在接收数据时，先接收数据的长度，
//然后根据长度接收实际的数据内容。这样可以处理变长的数据传输。
//在接收服务器返回的登录结果时，先接收数据长度，再接收实际的数据内容，并封装到 train_t 结构体中，方便后续处理。

//函数recvn的作用是确保从套接字 sockfd 接收到指定长度的字节数 len，并将接收到的数据存储在缓冲区 buf 中。
//它通过循环接收数据，直到接收到所需的字节数，sockfd->buf
void recvn(int sockfd, char buf[], int len)//recvn是自定义函数，recv是POSIX库函数
{
    int left = len;
    int ret = 0;
    while(left > 0)
    {
        ret = recv(sockfd, buf, left, 0);//recv单次接收数据，返回实际接收到的字节数，可能少于请求的长度。
        left -= ret;//left=len是函数传参需要传的总字节数，总left-ret成功收到的字节数=等于实际还要传的字节数
        buf += ret;//用于更新缓冲区指针buf，使其指向下一个要存储数据的位置。
    }
}

void sendTrainNamePasswd(int sockfd, int id, string name, string passwd)//发送用户名和密码
{
    train_s train;//定义用于发送的结构体小火车
    train.data = name+passwd;
    train.len = name.size();
    train.id = id;
    //字节数+字节数+字符数，在 ASCII 编码的每个字符是 1 个字节，但utf8不是
    size_t buffer_size = sizeof(train.len) + sizeof(train.id) + train.data.length();//4+4+字符串的长度，如abc就是4+4+3，不含\0
    //std::string 类提供了一个名为 length 的成员函数，用于获取字符串的长度，即字符串中字符的数量（不包括末尾的空字符 \0）

    //多分配了一个字节，用于存储换行符 \n。这个换行符被添加到缓冲区的最后一个字节，以确保数据发送到服务器时能够正确识别结束
    char* buffer = new char[buffer_size + 1]();//为buffer多申请一个空间，存memcpy的\0或\n，保证是一个有效字符串

    //// 1. 复制 train.len 到 buffer 的起始位置,复制的是&train.len存储的地址里的值4
    std::memcpy(buffer, &train.len, sizeof(train.len));//void *memcpy(void *dest, const void *src, size_t n);
    //04 00 00 00 是整数 4 的字节表示（假设小端字节序）

    // 2. 复制 train.id 到 buffer 的偏移位置（在 train.len 之后）
    std::memcpy(buffer + sizeof(train.len), &train.id, sizeof(train.id));//4+4+
    //// 3. 复制 train.data 到 buffer 的偏移位置（在 train.len 和 train.id 之后）
    std::memcpy(buffer + sizeof(train.len) + sizeof(train.id), train.data.c_str(), train.data.length());//
    //

    /* cout << "buffer_size: " << buffer_size << endl; */
    /* cout << "buffer: " << buffer+8 << endl; */
    // 服务器是接收到换行才会停止
    buffer[buffer_size] = '\n';// 4. 在 buffer 末尾添加换行符

    if (send(sockfd, buffer, buffer_size + 1, 0) < 0) {
        std::cout << "-------Error: Send failed!-------" << std::endl;
    }
    delete [] buffer;
}

//train_t& recvTrain 是一个函数参数，它的类型是 train_t&，表示对 train_t 类型的对象的引用。
void receiveTrainLogin(int sockfd, train_t& recvTrain)//接收服务端返回信息
{
    // 从套接字中接收数据长度并存储在 recvTrain.len 中
    if(recv(sockfd, &recvTrain.len, sizeof(int), 0) < 0)
    {
        cout << "-------Error: Receive failed!-------" << endl;
    }
    int left = recvTrain.len;// 获取数据长度
    int data;// 用于临时存储数据

    // 从套接字中接收实际的数据，并存储在 data 中
    if(recv(sockfd, &data, left, 0) < 0)//sockfd->data
    {
        cout << "-------Error: Receive failed!-------" << endl;
    }
    //reinterpret_cast 被用来将一个整数指针&data转换为字符指针char*
    recvTrain.data.append(reinterpret_cast<char*>(&data), sizeof(data));//string类的append
    //这里的string类的append方法用于将data变量的二进制内容追加到 recvTrain.data 字符串的末尾
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
    int cnt = 3;// 密码输错的最多次数

    while(cnt > 0)
    {
        //先发送1给服务器
        int dataId = 3;//dataId = 3 表示这是一个登录请求
        //输入用户名和密码
        string name, passwd;
        getNameAndPasswd(name, passwd);
        
        //小火车发送用户名和密码
        sendTrainNamePasswd(sockfd, dataId, name, passwd);
        
        //成功服务器返回0，用户名错误返回1，密码错误返回2
        train_t result;
        receiveTrainLogin(sockfd, result);
        /* cout << "ret: " << *(int*)(result.data.data()) << endl; */
        //返回的指针强制转为int*型指针，并取值用星号*进行取值
        int ret = *(int*)(result.data.data());//string类的成员函数const char* data() const noexcept; char* data() noexcept;
        //在现代 C++（C++11 及之后）中，data 函数返回的指针末尾有空字符，使用起来更加安全和方便，与 c_str 返回的指针相同，C++11之前最好用c_str
        //result.data.data() 获取字符串内容的指针，这个是result结构体的成员string类的data对象的成员函数data()
        //noexcept 是 C++11 引入的一个关键词，用于指示函数在其声明中是否保证不抛出异常。如果一个函数被标记为 noexcept，那么它承诺
        //不会抛出任何异常。如果函数确实抛出了异常，则程序会调用 std::terminate 终止程序执行
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
    /* printVersion(); */
    
    /* nlohmann::json jsonData1; */

    /* jsonData1 = {"wang", "jingji", "国内", "经济"}; */

    /* int depth1 = 0; */
    /* std::cout <<std::setfill('-')<<std::setw(46)<<"-"; */
    /* std::cout <<std::setfill(' ')<<std::setw(5)<<"\033[" << COLOR_CYAN  << "<HOT WORD LIST>" << "\033[0m"<<std::setw(5)<<""; */
    /* std::cout <<std::setfill('-')<<std::setw(46)<<"-"<<std::endl; */
    /* std::this_thread::sleep_for(std::chrono::milliseconds(200)); */
    /* for(const auto& val:jsonData1){ */
    /*     std::string stringValue = val; */
    /*     //std::cout << stringValue << std::endl; */
    /*     printWithGradientColor(stringValue,depth1); */
    /*     ++depth1; */
    /* } */
    
    // ./a.out ip port
    // ARGS_CHECK(argc,3);
    if (argc != 3) {//检测输入编译信息
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
        //std::cout << stringValue << std::endl;
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
                /* send(sockfd,&histo,sizeof(histo),0); */

                sendTrainNamePasswd(sockfd, histo, name, string());

                int len=0;
                recv(sockfd,&len,sizeof(len),0);
                char buf[len];
                memset(buf,0,sizeof(buf));
                recv(sockfd,buf,len,0);//接收历史记录

                std::cout << endl <<"-------View History-------"<<std::endl;
                std::string jsonstring(buf, len);
                /* cout << "buf: " << buf << endl; */
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



//有点问题暂存调试
void runSearch(size_t sockfd, string& name) {//用户进入搜索循环
    while (true) {
        // 获取搜索方式
        std::cout << "Please select the method you want to search: [keyword(1)][webpage(2)][quit(0)]:";//用户选择搜索方式
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
            size_t buffer_size = sizeof(t1.len) + sizeof(t1.id) + t1.data.length();//将输入的信息转换成字符流进行发送
            
            /* cout << "buffer_size: " << buffer_size << endl; */

            char* buffer = new char[buffer_size + 1]();
            std::memcpy(buffer, &t1.len, sizeof(t1.len));

            /* cout << "buffer_ti.len: " << *(int*)buffer << endl; */

            std::memcpy(buffer + sizeof(t1.len), &t1.id, sizeof(t1.id));

            /* cout << "buffer_t1.id: " << *(int*)(buffer+4) << endl; */

            std::memcpy(buffer + sizeof(t1.len) + sizeof(t1.id), t1.data.c_str(), t1.data.length());

            /* cout << "buffer_t1.data: " << (buffer + 8) << endl; */

            buffer[buffer_size] = '\n';

            // 发送字节流
            if (send(sockfd, buffer, buffer_size + 1, 0) < 0) {
                std::cout << "-------Error: Send failed!-------" << std::endl;
                continue;
            }

            delete[] buffer;//释放空间

            int len = 0;
            recv(sockfd, &len, sizeof(len), 0);
            char buf[len];
            memset(buf, 0, sizeof(buf));
            recv(sockfd, buf, len, 0);//接收服务端返回信息
            std::string Jsonstring(buf,len);

            /* cout << "jsonstring: " << Jsonstring << endl; */
            //Jsonstring.erase(Jsonstring.size()-1);
            //std::string test = R"(["string1","string2","string3","string4","string5","string6"])";
            //std::string test = R"(Jsonstring)";
            nlohmann::json jsonData = nlohmann::json::parse(Jsonstring);
            //nlohmann::json jsonData = nlohmann::json::parse(test);
            //cout << jsonData.dump() << endl; 

            /* for (auto it = jsonData.rbegin(); it != jsonData.rend(); ++it) */
            /* { */
            /*     std::cout << *it << std::endl; */
            /* } */
            cout << endl;
            for (const auto& val : jsonData) {
                std::string stringValue = val;
                std::cout << stringValue << std::endl;
            }
            cout << endl;
        }
        else if (choice == 2) { // web pages
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
            t1.id = 2;
            t1.data = name + line;
            t1.len = name.length();

            // 转换为字节流
            size_t buffer_size = sizeof(t1.len) + sizeof(t1.id) + t1.data.length();
            char* buffer = new char[buffer_size+1]();//将数据打包字符流尽心发送
            std::memcpy(buffer, &t1.len, sizeof(t1.len));
            std::memcpy(buffer + sizeof(t1.len), &t1.id, sizeof(t1.id));
            std::memcpy(buffer + sizeof(t1.len) + sizeof(t1.id), t1.data.c_str(), t1.data.length());
            /* std::cout << "buffer_t1.data: " << (buffer + 8) << std::endl; */
            buffer[buffer_size] = '\n';

            // 发送字节流
            if (send(sockfd, buffer, buffer_size+1, 0) < 0) {
                std::cout << "-------Error: Send failed!-------" << std::endl;
                continue;
            }

            delete[] buffer;


            int len = 0;
            recv(sockfd, &len, sizeof(len), 0);
            char buf[len + 1];
            memset(buf, 0, sizeof(buf));
            recvn(sockfd, buf, len);

            std::string jsonString(buf,len);
            nlohmann::json jsonData = nlohmann::json::parse(jsonString);//接收解析客户端返回的网页信息
            cout << endl;
            for (const auto& doc : jsonData) {//输出网页信息
                std::string Title = doc["_docTitle"].get<std::string>();
                std::string URL = doc["_docUrl"].get<std::string>();
                std::string Sum = doc["_docSummary"].get<std::string>();
                std::cout << "Title: " << Title << std::endl;
                std::cout << "URL: " << URL << std::endl;
                std::cout << "Summary: " << Sum << std::endl;
                /* outRedcolor(Sum, line); */
                std::cout << "------------------------------------------" << std::endl;
            }
            cout << endl;
        }
        else if(choice==0){
            system("clear");
            sleep(5);
            system("ascii-image-converter -C ./pic.png");
            exit(1);
        }
        else {
            std::cout << "-------Error: Please enter a valid number (1 or 2)!-------" << std::endl;
            continue;
        }
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
