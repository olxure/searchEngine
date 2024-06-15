#ifndef __SPLITTOOL_H__
#define __SPLITTOOL_H__

#include <vector>
#include <string>
using std::vector;
using std::string;

class SplitTool
{
public:
    virtual ~SplitTool(){}  //虚析构函数
    virtual vector<string> cut(const string &sentence) = 0;
};
#endif