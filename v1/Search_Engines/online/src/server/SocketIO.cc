#include "SocketIO.h"
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

SocketIO::SocketIO(int fd)
: _fd(fd)
{

}

SocketIO::~SocketIO()
{
    close(_fd);
}

//len = 10000    1500/6     1000/1
int SocketIO::readn(char *buf, int len)
{
    int left = len;
    char *pstr = buf;
    int ret = 0;

    while(left > 0)
    {
        ret = read(_fd, pstr, left);
        if(-1 == ret && errno == EINTR)//EINTR中断错误
        {
            continue;
        }
        else if(-1 == ret)
        {
            perror("read error -1");
            return len - ret;
        }
        else if(0 == ret)
        {
            break;
        }
        else
        {
            pstr += ret;
            left -= ret;
        }
    }

    return len - left;
}

//从套接字中读取一行数据，并将其存储在缓冲区buf中。这个函数使用了recv函数和MSG_PEEK标志来窥探数据，而不会将其从内核缓冲区中移除。
int SocketIO::readLine(char *buf, int len)
{
    int left = len - 1;
    char *pstr = buf;
    int ret = 0, total = 0;

    while(left > 0)
    {
        //MSG_PEEK不会将内核缓冲区中的数据进行清空,只会进行拷贝操作
        ret = recv(_fd, pstr, left, MSG_PEEK);
        if(-1 == ret && errno == EINTR)
        {
            continue;
        }
        else if(-1 == ret)
        {
            std::cout << "perror" << std::endl;
            perror("readLine error -1");
            return len - ret;
        }
        else if(0 == ret)
        {
            break;
        }
        else
        {//第二层for循环
            for(int idx = 0; idx < ret; ++idx)
            //在接收到的数据中查找换行符（\n），以确定一行的结束位置。一旦找到换行符，
            //它会实际读取该行的数据（从内核缓冲区中移除），并将其存储在用户缓冲区中。
            {
                if(pstr[idx] == '\n')
                {
                    int sz = idx + 1;
                    readn(pstr, sz);
                    
                    // using std::cout;
                    // using std::endl;
                    // cout << "length: " << *(int*)(pstr) << endl;
                    // cout << "id: " << *(int*)(pstr + 4) << endl;
                    // cout << "data: " << (pstr+8) << endl;

                    pstr += sz - 1;
                    *pstr = '\0';//C风格字符串以'\0'结尾

                    return total + sz;
                }
            }
            readn(pstr, ret);//从内核态拷贝到用户态
            std::cout<< "======" << std::endl;
            std::cout << "ret: " << ret << "left: " << left << std::endl;
            total += ret;
            pstr += ret;
            left -= ret;
        }
    }//while循环结束
    *pstr = '\0';
    //total累计已经读取的字节数，left剩余需要读取的字节数
    return total - left;

}//int SocketIO::readLine(char *buf, int len)结束

//标准库中的write函数不能保证一次性将所有数据写完，自定义writen通常是用来解决这个问题的。
int SocketIO::writen(const char *buf, int len)//返回值是实际写入的字节数，这可以用于确认数据是否完全写入
{
    int left = len;
    const char *pstr = buf;
    int ret = 0;

    while(left > 0)
    {
        ret = write(_fd, pstr, left);
        if(-1 == ret && errno == EINTR)
        {
            continue;// 如果被信号中断，重新写入
        }
        else if(-1 == ret)
        {
            perror("writen error -1");
            return len - ret;// 返回已经写入的字节数
            /* return -1; */
        }
        else if(0 == ret)
        {
            break;// 写入0字节，退出循环
        }
        else
        {
            pstr += ret;// 更新指针位置
            left -= ret;// 更新剩余字节数要写入
        }
    }//while循环结束

    return len - left;// 返回实际写入的字节数
}
