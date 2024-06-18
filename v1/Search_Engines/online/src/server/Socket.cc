#include "Socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
Socket::Socket()
{
    _fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if(_fd < 0)
    {
        perror("socket");
        return;
    }
    std::cout << "Socket created with fd: " << _fd << std::endl;//测试看fd是否生成正常
}

Socket::Socket(int fd)
: _fd(fd)
{
    std::cout << "Socket initialized with existing fd: " << fd << std::endl;//测试看fd是否生成正常
}

Socket::~Socket()
{
    std::cout << "Closing socket with fd: " << _fd << std::endl;//测试看fd是否生成正常
    close(_fd);
}

int Socket::fd() const
{
    return _fd;
}

void Socket::shutDownWrite()
{
    //关闭写端
    int ret = shutdown(_fd, SHUT_WR);
    //SHUT_RD：关闭读端 ；  SHUT_WR：关闭写端；  SHUT_RDWR：同时关闭读端和写端。
    if(ret)
    {
        perror("shutdown");
        return;
    }
    std::cout << "Socket write shutdown on fd: " << _fd << std::endl;//测试看fd是否生成正常
}
