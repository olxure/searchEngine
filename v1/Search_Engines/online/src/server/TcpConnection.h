#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#define __MAXMESSAGELENGTH__ 65535

#include "Socket.h"
#include "SocketIO.h"
#include "InetAddress.h"
#include <memory>
#include <functional>

using std::shared_ptr;
using std::function;

class EventLoop;//前向声明

class TcpConnection
: public std::enable_shared_from_this<TcpConnection>
{
    using TcpConnectionPtr = shared_ptr<TcpConnection>;
    using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;
public:
    explicit TcpConnection(int fd, EventLoop *loop);
    ~TcpConnection();
    void send(const string &msg);// 发送消息
    void sendInLoop(const string& msg);//让msg在EventLoop发送 // 在事件循环中发送消息
    char* receive(); // 接收消息
    
    string toString();// 生成描述TCP连接的字符串（调试用）
    
    bool isClosed() const;// 判断连接是否已断开
    
    //在TcpConnection中需要注册三个回调函数
    void setNewConnectionCallback(const TcpConnectionCallback &cb);
    void setMessageCallback(const TcpConnectionCallback &cb);
    void setCloseCallback(const TcpConnectionCallback &cb);
    void setTimeoutCallback(const TcpConnectionCallback &cb);

    //执行对应的三个回调函数
    void handleNewConnectionCallback();// 新连接回调
    void handleMessageCallback();// 消息回调
    void handleCloseCallback();// 连接关闭回调
    void handleTimeOutCallback();// 超时回调

private:
    InetAddress getLocalAddr();// 获取本地地址
    InetAddress getPeerAddr();// 获取对端地址

private:
    EventLoop *_loop;//让TcpConnection知道EventLoop的存在
    SocketIO _sockIO;

    //为了调试而加入的函数
    Socket _sock;
    InetAddress _localAddr;//两个包含IP和端口等的结构体
    InetAddress _peerAddr;//两个包含IP和端口等的结构体

    TcpConnectionCallback _onConnectionCb;//1、连接建立
    TcpConnectionCallback _onMessageCb;//2、消息到达
    TcpConnectionCallback _onCloseCb;//3、连接断开
    TcpConnectionCallback _onTimeOutCb; //4.定时器触发
};

#endif
