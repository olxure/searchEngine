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
