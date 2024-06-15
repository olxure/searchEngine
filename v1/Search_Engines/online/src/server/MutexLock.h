#ifndef __MUTEXLOCK_H__
#define __MUTEXLOCK_H__

#include "NonCopyable.h"
#include <pthread.h>

class MutexLock
: NonCopyable
{
public:
    MutexLock();
    ~MutexLock();
    void lock();
    void tryLock();
    void unlock();

    pthread_mutex_t *getMutexLockPtr()
    {
        return &_mutex;
    }

private:
    pthread_mutex_t _mutex;//

};

//本质：利用栈对象的生命周期管理资源
class MutexLockGuard
{
public:
    //在构造函数中初始化资源
    MutexLockGuard(MutexLock &mutex)
    : _mutex(mutex)
    {
        _mutex.lock();
    }

    //在析构函数中释放资源
    ~MutexLockGuard()
    {
        _mutex.unlock();
    }
private:
    MutexLock &_mutex;
};

#endif
