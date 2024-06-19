#include "Thread.h"

#include <stdio.h>
//在 C++11 及之后的标准中，建议使用 thread_local 代替 __thread，因为 thread_local 是标准的并且可移植的，而__thread是 GCC 和 Clang 编译器特有的
//在 C++ 中，__thread 是一个线程局部存储（Thread Local Storage, TLS）的关键字。它用于声明一个变量，使得每个线程都有该变量的独立实例
//换句话说，每个线程都有自己的 name 变量，而不是所有线程共享一个 name 变量。这在多线程编程中非常有用，尤其是在需要每个线程维护自己的状态或数据时。
namespace current_thread {
    __thread const char * name;// 声明线程局部存储变量
}

//using Thread::ThreadCallback = std::function<void ()>  构造函数 接收函数对象和常量字符串
Thread::Thread(ThreadCallback &&cb, const string &name)
: _thid(0)// 初始化线程ID
, _name(name)// 初始化线程名称
, _isRunning(false)// 初始化运行状态

//将传入的 cb（类型为 ThreadCallback，即 std::function<void()>）转换为右值引用。
, _cb(std::move(cb))//注册回调函数，// 移动构造回调函数
//调用 std::function<void()> 的移动构造函数，将 cb 的资源转移给 _cb。
//这样 _cb 直接接管了 cb 所持有的资源，而不需要深拷贝，从而提高效率。
{

}

Thread::~Thread()
{

}

//线程运行与停止的函数
void Thread::start()
{
    //this是将当前对象的指针传递给了新创建的线程。这样，新线程在执行 threadFunc 时，可以通过这个指针访问当前对象的成员
    //this 指向的是调用 start 方法的 Thread 对象。
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
void *Thread::threadFunc(void *arg)//传递过来的是指向Thread对象的this指针，即指向Thread对象的指针
{   //相当于arg->Thread
    /* arg->run();//error */
    Thread *pth = static_cast<Thread *>(arg);
    if(pth)
    {
        //修改子线程的name,这个_name是在调用Thread的构造函数的时候初始化的
        current_thread::name = pth->_name.c_str();
        //执行任务
        pth->_cb();//调用线程的回调函数，也就是线程需要执行的具体任务，这
        //虽然这里的回调函数并没有使用传统意义上的多态（继承和虚函数），但仍然是多态的一种形式。
        //回调函数可以是任何符合 std::function<void()> 类型的函数对象
        //意味着可以将不同的任务分配给不同的线程，而不需要修改线程类的代码。这就是所谓的“多态性”：
        //同一个接口（这里是 std::function<void()>）可以指向不同的实现
    }
    else
    {
        printf("hello\n");
    }

    pthread_exit(nullptr);
    /* return nullptr; */
}

