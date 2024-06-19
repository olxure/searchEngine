#include "EchoServer.h"
#include <iostream>

using std::cout;
using std::endl;

Configuration* Configuration::_pInstance = nullptr;//初始化一个指向Configuration类的指针为空
pthread_once_t Configuration::_once = PTHREAD_ONCE_INIT;

//Configuration::_initFunc：这是 Configuration 类的一个静态成员变量。它是一个函数对象，
//可以存储一个返回类型为 void、不接受任何参数的可调用目标
function<void()> Configuration::_initFunc;
//实际的初始化发生在代码的其他部分，例如在 getInstance 方法中：通过 std::bind 或其他方式完成
//注意，这里不是函数对象的初始化，它只是声明了 _initFunc 是 Configuration 类的一个静态成员，
//并且它将存储一个可以调用的对象，该对象的类型是 void()

int main(int argc, char *argv[])
{
    EchoServer es(4, 10, "127.0.0.1", 8888, argv[1]);
    //4: 表示线程池的线程数。这意味着服务器将在后台运行 4 个线程来处理客户端请求。
    //10: 表示任务队列的最大长度。任务队列用于存储待处理的客户端请求，最大可容纳 10 个任务
    es.start();//启动类，启动了很多工作

    return 0;
}

