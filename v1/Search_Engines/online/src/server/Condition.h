#ifndef __CONDITION_H__
#define __CONDITION_H__

#include "NonCopyable.h"
/* #include "MutexLock.h" //防止循环依赖 */
#include <pthread.h>

class MutexLock;//前向声明

class Condition
: NonCopyable
{
public:
    Condition(MutexLock &mutex);
    ~Condition();
    void wait();
    void notify();
    void notifyAll();

private:
    MutexLock &_mutex;
    pthread_cond_t _cond;//

};

#endif
