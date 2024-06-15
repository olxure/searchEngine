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
    using ThreadCallback = function<void()>;
public:
    Thread(ThreadCallback &&cb, const string & name = "wd thread");
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
    ThreadCallback _cb;//就是需要执行的任务

};

#endif
