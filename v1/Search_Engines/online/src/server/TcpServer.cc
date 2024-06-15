#include "TcpServer.h"

TcpServer::TcpServer(const string &ip, unsigned short port)
: _acceptor(ip, port)
, _loop(_acceptor)
{

}

TcpServer::~TcpServer()
{

}

//让服务器运行起来
void TcpServer::start()
{
    _acceptor.ready();
    _loop.loop();
}

//让服务器停止
void TcpServer::stop()
{
    _loop.unloop();
}

void TcpServer::setAllCallback(TcpConnectionCallback &&onConnection
                               , TcpConnectionCallback &&onMessage
                               , TcpConnectionCallback &&onClose
                               , TcpConnectionCallback &&onTimeout)
{
    _loop.setNewConnectionCallback(std::move(onConnection));
    _loop.setMessageCallback(std::move(onMessage));
    _loop.setCloseCallback(std::move(onClose));
    _loop.setTimeOutCallback(std::move(onTimeout));
}

