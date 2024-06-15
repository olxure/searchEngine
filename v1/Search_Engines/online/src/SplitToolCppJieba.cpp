#include "../include/include/SplitToolCppJieba.h"


SplitToolCppJieba * SplitToolCppJieba::_pInstance = nullptr;//懒汉/饱汉(懒加载)模式
pthread_once_t SplitToolCppJieba::_once = PTHREAD_ONCE_INIT;
function<void()> SplitToolCppJieba::_initFunc;
