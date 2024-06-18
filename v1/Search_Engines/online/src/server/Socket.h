#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "NonCopyable.h"

// Socket类，继承自NonCopyable
class Socket
: NonCopyable
{
public:
    Socket();// 默认构造函数

    //不能隐式地将整数转换为Socket对象，必须显式调用构造函数
    explicit Socket(int fd);// 带参数的构造函数，接受一个文件描述符
    ~Socket();
    //常量成员函数承诺不会修改所属对象的状态，也就是说，它不会修改类的任何成员变量,如数据成员_fd
    int fd() const;// 返回Socket的文件描述符
    void shutDownWrite();//关闭套接字的写端，禁止发送更多数据，但仍可以接收数据

private:
    int _fd;// 保存Socket的文件描述符
};

#endif
