#ifndef __SOCKETIO_H__
#define __SOCKETIO_H__

class SocketIO
{
public:
    explicit SocketIO(int fd);
    ~SocketIO();
    int readn(char *buf, int len);
    int readLine(char *buf, int len);

    //返回值是实际写入的字节数。这可以用于确认数据是否完全写入，len是本来要写入的字节数
    int writen(const char *buf, int len);

private:
    int _fd;
};

#endif
