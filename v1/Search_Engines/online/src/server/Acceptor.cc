#include <iostream>
#include "Acceptor.h"
#include <stdio.h>

//// 构造函数，初始化Socket和InetAddress
Acceptor::Acceptor(const string &ip, unsigned short port)
: _sock()// 初始化Socket对象
, _addr(ip, port)// 初始化InetAddress对象，保存IP和端口
//struct sockaddr_in _addr;// 存储IP地址和端口的结构体
{
}

Acceptor::~Acceptor()
{

}

// 准备监听Socket，相关的函数前向声明
void Acceptor::ready()
{
    setReuseAddr();// 设置地址重用
    setReusePort();// 设置端口重用
    bind();// 绑定IP和端口
    listen();// 开始监听
}

// 设置地址重用
void Acceptor::setReuseAddr()
{
    int on = 1;// 定义一个整数变量`on`并将其值设置为1，用于启用套接字选项
    //在调用 setsockopt 之前，确保套接字已经创建并绑定到地址，这个网络编程函数详见笔记
    int ret = setsockopt(_sock.fd(), SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    // 调用`setsockopt`函数设置套接字选项：
    // - 第一个参数是套接字文件描述符，通过调用`_sock.fd()`获取。
    // - 第二个参数是选项的级别，这里是`SOL_SOCKET`，表示在套接字级别上设置选项。
    // - 第三个参数是选项名称，这里是`SO_REUSEADDR`，表示地址重用。
    // - 第四个参数是指向包含选项值的缓冲区，这里是指向`on`变量的指针。
    // - 第五个参数是选项值的大小，这里是`sizeof(on)`。
    //setReuseAddr方法通过调用setsockopt函数设置SO_REUSEADDR选项，以允许在一个端口上重新绑定。
    //这样做的主要目的是允许服务器在快速重启时避免“地址已在使用中”的错误
    if(ret)
    {
        perror("setsockopt");
        return;
    }
}

// 设置端口重用
void Acceptor::setReusePort()
{
    int on = 1;
    int ret = setsockopt(_sock.fd(), SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    if(-1 == ret)
    {
        perror("setsockopt");
        return;
    }
}

// 绑定IP和端口
void Acceptor::bind()
{
    int ret = ::bind(_sock.fd(), (struct sockaddr *)&_addr, sizeof(struct sockaddr));
    //int ret = ::bind(_sock.fd(), (struct sockaddr *)_addr.getInetAddrPtr(),sizeof(struct sockaddr));//可能有点问题暂存
    if(-1 == ret)
    {
        perror("bind");
        std::cerr << "Failed to bind socket with fd: " << _sock.fd() << std::endl;
        return;
    }
    std::cerr << "Socket bound successfully with fd: " << _sock.fd() << std::endl;
}

// 开始监听
void Acceptor::listen()
{
    int ret = ::listen(_sock.fd(), 128);// 最大监听队列为128
    if(-1 == ret)
    {
        perror("listen");
        std::cerr << "Failed to listen on socket with fd: " << _sock.fd() << std::endl;
        return;
    }
    std::cerr << "Socket is now listening with fd: " << _sock.fd() << std::endl;
}

// 接受新的连接
int Acceptor::accept()
{
    //::accept 使用了全局作用域解析运算符，表示调用全局命名空间中的 accept 函数。
    //而不是类的成员函数或其他命名空间中的函数
    int connfd = ::accept(_sock.fd(), nullptr, nullptr);
    //_sock.fd()：调用 Socket 类的 fd 方法，返回套接字文件描述符。
    //if(-1 == connfd)//有点问题暂存
    if (connfd < 0)
    {
        perror("accept");
        return -1;
    }
    std::cout << "Accepted connection with fd: " << connfd << std::endl;//测试fd
    return connfd;// 返回新连接的文件描述符
}


// 返回监听Socket的文件描述符
int Acceptor::fd() const
{
    return _sock.fd();
}
