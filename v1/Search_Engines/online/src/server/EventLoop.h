#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include "MutexLock.h"
#include <vector>
#include <map>
#include <memory>
#include <functional>

using std::vector;
using std::map;
using std::shared_ptr;
using std::function;
using std::bind;

class Acceptor;//前向声明
class TcpConnection;

using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;

class EventLoop
{
    using Functors = function<void()>;

public:
    EventLoop(Acceptor &acceptor);
    ~EventLoop();

    //是否循环
    void loop();
    void unloop();

    void runInLoop(Functors &&cb);

    //在EventLoop中需要注册三个回调函数，将其传递给TcpConnection
    void setNewConnectionCallback(TcpConnectionCallback &&cb);
    void setMessageCallback(TcpConnectionCallback &&cb);
    void setCloseCallback(TcpConnectionCallback &&cb);
    void setTimeOutCallback(TcpConnectionCallback &&cb);

private:
    int createTimeFd();
    void setTimerFd(int initSec, int peridocSec);

private:
    //执行epoll_wait的函数
    void waitEpollFd();
    //处理新的连接
    void handleNewConnection();
    //处理消息的发送
    void handleMessage(int fd);
    //创建文件描述符epfd
    void handleTimeOut(int fd);
    int createEpollFd();
    //将文件描述符放在红黑树上进行监听
    void addEpollReadFd(int fd);
    //将文件描述符从红黑树上删除
    void delEpollReadFd(int fd);

private:
    void handleRead();
    void wakeup();
    int createEventFd();
    void doPendingFunctors();//将vector中的所有回调函数进行执行

private:
    int _epfd;//红黑树上的节点
    int _evfd;//eventfd系统调用创建的文件描述符
    bool _isLooping;//标识循环是否进行
    Acceptor &_acceptor;//为了调用Acceptor中的accept函数
    vector<struct epoll_event> _evtList;//存放满足条件的文件描述符
    map<int, TcpConnectionPtr> _conns;//存储文件描述符与TcpConnection的键值对

    TcpConnectionCallback _onConnectionCb;//1、连接建立
    TcpConnectionCallback _onMessageCb;//2、消息到达
    TcpConnectionCallback _onCloseCb;//3、连接断开
    TcpConnectionCallback _onTimeOutCb;//4.定时器触发

    vector<Functors> _pendings;//待执行的任务的数组
    MutexLock _mutex;//vector是共享资源，需要加锁

    // 定时器
    int _timerfd;       // timer文件描述符
    int _initSec;   // 定时器的初始时间
    int _peridocSec;    // 定时器的周期时间
};

#endif
