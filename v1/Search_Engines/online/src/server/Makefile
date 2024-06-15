# 定义编译器和编译选项
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

# 定义库链接选项
LDFLAGS = -lpthread -llog4cpp -lmysqlclient -ljsoncpp -lcrypt -lhiredis -ltinyxml2

# 定义源文件
SRCS = TestEchoServer.cc \
       Acceptor.cc \
       Condition.cc \
       EventLoop.cc \
       InetAddress.cc \
       MutexLock.cc \
       MyTask.cc \
       Socket.cc \
       SocketIO.cc \
       TaskQueue.cc \
       TcpConnection.cc \
       TcpServer.cc \
       Thread.cc \
       ThreadPool.cc \
       ../CacheManager.cpp \
       ../Dictionary.cpp \
       ../KeyRecommander.cpp \
       ../LRUCache.cpp \
       ../Mylogger.cpp \
       ../MyRedis.cpp \
       ../WebPageQuery.cpp \
       ../SplitToolCppJieba.cpp \
       ../WebLib.cpp

# 定义目标可执行文件
TARGET = TestEchoServer

# 默认目标
all: $(TARGET)

# 编译并链接源文件生成可执行文件
$(TARGET):
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# 清理
clean:
	rm -f $(TARGET)

# 伪目标
.PHONY: all clean
