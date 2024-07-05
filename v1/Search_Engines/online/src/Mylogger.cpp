#include "../include/Mylogger.hpp"
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/OstreamAppender.hh>

using namespace log4cpp;

Mylogger* Mylogger::_pInstance = nullptr;
pthread_once_t Mylogger::_once = PTHREAD_ONCE_INIT;

Mylogger::Mylogger()
:_mylog(Category::getRoot().getInstance("mylog"))
{
    //// 设置日志输出格式
    PatternLayout* ptn = new PatternLayout();
    ptn->setConversionPattern("%d %c [%p] %m%n");
    PatternLayout* ptn2 = new PatternLayout();
    ptn2->setConversionPattern("%d %c [%p] %m%n");

    //// 创建滚动文件附加器
    RollingFileAppender* rollfile = new RollingFileAppender
        ("rollingfile", "/home/lexu/searchEngine/v1/Search_Engines/log/wd.log", 1024, 5, true);  // 使用绝对路径
        //("rollingfile", "../log/wd.log", 1024, 5, true);
    rollfile->setLayout(ptn);
    
    //// 创建控制台附加器
    OstreamAppender* osApp = new OstreamAppender
        ("console", &cout);
    osApp->setLayout(ptn2);

    //// 设置日志优先级和附加器
    _mylog.setPriority(Priority::DEBUG);
    _mylog.setAppender(rollfile);
    _mylog.setAppender(osApp);
    cout << "Mylogger()" << endl;
}

Mylogger::~Mylogger()
{
    _mylog.shutdown();
    cout << "~Mylogger()" << endl;
}

