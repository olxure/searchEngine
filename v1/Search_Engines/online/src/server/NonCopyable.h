#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

class NonCopyable
{
/* public: */
protected:
    NonCopyable()
    {
    }
    ~NonCopyable()
    {
    }

private:
    NonCopyable(const NonCopyable &rhs);
    NonCopyable &operator=(const NonCopyable &rhs);

private:

};

#endif
