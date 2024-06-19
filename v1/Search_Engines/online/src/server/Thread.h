#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include <functional>
#include <string>

using std::function;
using std::bind;
using std::string;

namespace current_thread {
    extern __thread const char * name;// int
}

class Thread
{
    using ThreadCallback = function<void()>;//设置线程回调函数的快速命名声明变量方式
public:
    //这是一个右值引用参数，类型为 ThreadCallback，它实际上是一个 std::function<void()> 类型的函数对象
    Thread(ThreadCallback &&cb, const string & name = "olx thread");
    ~Thread();

    //线程运行与停止的函数
    void start();
    void join();

private:
    //线程入口函数
    static void *threadFunc(void *arg);

private:
    pthread_t _thid;//线程id
    string _name;
    bool _isRunning;//标识线程是否在运行
    ThreadCallback _cb;//using Thread::ThreadCallback = std::function<void ()>

};

#endif
