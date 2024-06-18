#ifndef __CONDITION_H__
#define __CONDITION_H__

#include "NonCopyable.h"// 包含NonCopyable基类的定义
/* #include "MutexLock.h" //防止循环依赖 */
#include <pthread.h>// 包含POSIX线程库的定义

class MutexLock;//前向声明MutexLock类

class Condition: NonCopyable//默认私有继承
{
public:
    Condition(MutexLock &mutex);//
    ~Condition();
    void wait();
    void notify();
    void notifyAll();

private:
    MutexLock &_mutex;
    pthread_cond_t _cond;//

};

#endif
