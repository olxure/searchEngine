#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

class NonCopyable
{
/* public: */
protected://以下成员函数只能被该类及其派生类访问
    NonCopyable()// 默认构造函数，允许派生类构造
    {
    }
    ~NonCopyable()// 默认析构函数，允许派生类析构
    {
    }

private://只能被该类自身访问，外部和派生类都无法访问
    NonCopyable(const NonCopyable &rhs);// 拷贝构造函数被声明为私有并且没有实现
    NonCopyable &operator=(const NonCopyable &rhs);// 赋值运算符被声明为私有并且没有实现

private:

};

#endif
