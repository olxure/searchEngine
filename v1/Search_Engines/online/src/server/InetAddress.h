#ifndef __INETADDRESS_H__
#define __INETADDRESS_H__

#include <arpa/inet.h>
#include <string>

using std::string;

class InetAddress
{
public:
    InetAddress(const string &ip, unsigned short port);// 构造函数，接受IP地址和端口
    InetAddress(const struct sockaddr_in &addr);// 构造函数，接受struct sockaddr_in结构
    ~InetAddress();
    string ip() const;// 返回IP地址
    unsigned short port() const; // 返回端口号
    const struct sockaddr_in *getInetAddrPtr() const;// 返回sockaddr_in结构的指针

private:
    struct sockaddr_in _addr;// 存储IP地址和端口的结构体

};

#endif
