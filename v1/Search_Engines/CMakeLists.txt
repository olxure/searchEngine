cmake_minimum_required(VERSION 3.10)
project(SearchEngine)

set(CMAKE_CXX_STANDARD 17)

# 添加包含目录
include_directories(include)

# 添加源文件
file(GLOB SOURCES "src/*.cpp")

# 添加可执行文件
add_executable(SearchEngine ${SOURCES})

# 链接必需的库
find_package(MySQL REQUIRED)
find_package(Redis REQUIRED)
find_package(nlohmann_json REQUIRED)

target_link_libraries(SearchEngine PRIVATE MySQL::MySQL Redis::Redis nlohmann_json::nlohmann_json)

