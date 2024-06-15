#include "Thread.h"

#include <stdio.h>

namespace current_thread {
    __thread const char * name;// int
}

Thread::Thread(ThreadCallback &&cb, const string &name)
: _thid(0)
, _name(name)
, _isRunning(false)
, _cb(std::move(cb))//注册回调函数
{

}

Thread::~Thread()
{

}

//线程运行与停止的函数
void Thread::start()
{
    int ret = pthread_create(&_thid, nullptr, threadFunc, this);
    if(ret)
    {
        perror("pthread_create");
        return;
    }

    _isRunning = true;
}

void Thread::join()
{
    if(_isRunning)
    {
        int ret = pthread_join(_thid, nullptr);
        if(ret)
        {
            perror("pthread_join");
            return;
        }

        _isRunning = false;
    }
}

//线程入口函数
void *Thread::threadFunc(void *arg)
{
    /* arg->run();//error */
    Thread *pth = static_cast<Thread *>(arg);
    if(pth)
    {
        //修改子线程的name
        current_thread::name = pth->_name.c_str();
        //执行任务
        pth->_cb();//多态
    }
    else
    {
        printf("hello\n");
    }

    pthread_exit(nullptr);
    /* return nullptr; */
}

