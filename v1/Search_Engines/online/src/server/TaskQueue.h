#ifndef __TASKQUEUE_H__
#define __TASKQUEUE_H__

#include "MutexLock.h"
#include "Condition.h"
#include <queue>
#include <functional>

using std::queue;
using std::function;

using ElemType = function<void()>;

class TaskQueue
{
public:
    TaskQueue(size_t queSize);
    ~TaskQueue();

    //任务队列空与满
    bool empty() const;
    bool full() const;

    //添加任务与执行任务
    void push(ElemType &&value);
    ElemType pop();

    //唤醒所有等待在_notEmpty条件变量上的线程
    /* void wakeupAllNotEmpty(); */
    void wakeup();
    
private:
    size_t _queSize;//任务队列的大小
    queue<ElemType> _que;//存放数据的数据结构
    MutexLock _mutex;//互斥锁
    Condition _notEmpty;//不空的条件变量
    Condition _notFull;//不满的条件变量
    bool _flag;//让工作线程唤醒的时候，可以退出来

};

#endif
