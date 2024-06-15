#include "TaskQueue.h"
#include <unistd.h>

TaskQueue::TaskQueue(size_t queSize)
: _queSize(queSize)
, _que()
, _mutex()
, _notEmpty(_mutex)
, _notFull(_mutex)
, _flag(true)
{

}

TaskQueue::~TaskQueue()
{

}

//任务队列空与满
bool TaskQueue::empty() const
{
    return 0 == _que.size();
}

bool TaskQueue::full() const
{
    return _queSize == _que.size();
}

//添加任务与执行任务
void TaskQueue::push(ElemType &&value)
{
    //RAII的思想
    //本质：利用栈对象的生命周期管理资源
    MutexLockGuard autoLock(_mutex);

    //2、判断是不是满的
    //虚假唤醒
    while(full())
    {
        //如果TaskQueue是满的，就不能存任务，
        //生产者线程就在该条件变量上睡眠
        _notFull.wait();
    }

    //3、存任务
    _que.push(std::move(value));

    //3.1、生产者生产任务之后，需要唤醒消费者
    _notEmpty.notify();
}

ElemType TaskQueue::pop()
{
    MutexLockGuard autoLock(_mutex);

    //2、是不是空的
    while(empty() &&_flag)
    {
        //如果任务队列为空，那么消费者
        //线程就需要睡眠
        _notEmpty.wait();
    }

    if(_flag)
    {
        //3、取任务
        ElemType tmp = _que.front();
        _que.pop();

        //3.1、唤醒生产者线程，让其生产任务
        _notFull.notify();
        return tmp;
    }
    else
    {
        return nullptr;
    }
}

void TaskQueue::wakeup()
{
    _flag = false;
    _notEmpty.notifyAll();
}
