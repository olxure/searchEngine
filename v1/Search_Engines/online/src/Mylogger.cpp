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
    PatternLayout* ptn = new PatternLayout();
    ptn->setConversionPattern("%d %c [%p] %m%n");
    PatternLayout* ptn2 = new PatternLayout();
    ptn2->setConversionPattern("%d %c [%p] %m%n");

    RollingFileAppender* rollfile = new RollingFileAppender
        ("rollingfile", "../log/wd.log", 1024, 5, true);
    rollfile->setLayout(ptn);
    
    OstreamAppender* osApp = new OstreamAppender
        ("console", &cout);
    osApp->setLayout(ptn2);

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

