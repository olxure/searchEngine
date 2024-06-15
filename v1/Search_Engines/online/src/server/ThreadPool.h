#ifndef __THREADPOLL_H__
#define __THREADPOLL_H__

#include "Thread.h"
#include "TaskQueue.h"
#include <vector>
#include <memory>
#include <functional>

using std::vector;
using std::unique_ptr;
using std::function;

class Thread;//前向声明

using Task = function<void()>;

class ThreadPool
{
public:
    ThreadPool(size_t threadNum, size_t queSize);
    ~ThreadPool();

    //线程池的启动与退出
    void start();
    void stop();

    //添加任务与获取任务
    void addTask(Task &&task);

private:
    Task getTask();
    //线程池交给每个具体线程做的任务，封装成了函数
    void doTask();

private:
    size_t _threadNum;//子线程的数目
    size_t _queSize;//任务队列的大小
    vector<unique_ptr<Thread>> _threads;//存放工作线程的容器
    TaskQueue _taskQue;//存放任务的数据结构
    bool _isExit;//线程池是否退出的标志
};

#endif
