#include "EchoServer.h"
#include <iostream>

using std::cout;
using std::endl;

Configuration* Configuration::_pInstance = nullptr;
pthread_once_t Configuration::_once = PTHREAD_ONCE_INIT;
function<void()> Configuration::_initFunc;

int main(int argc, char *argv[])
{
    EchoServer es(4, 10, "127.0.0.1", 8888, argv[1]);
    es.start();

    return 0;
}

