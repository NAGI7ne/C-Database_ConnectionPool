# cmake最低版本
cmake_minimum_required(VERSION 3.28)
project(CPP_dbConnectionPool)

#C++ 20
set(CMAKE_CXX_STANDARD 20)

#生成ycm可识别的json文件，让源文件找到头文件
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON )

#源文件，头文件路径
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC)
include_directories(${PROJECT_SOURCE_DIR}/inc)

#使用 configure_file 将 JSON 文件复制到构建目录
configure_file(${CMAKE_SOURCE_DIR}/dbconf.json ${CMAKE_BINARY_DIR}/dbconf.json COPYONLY)


#输出可执行文件路径
set(EXECUTABLE_OUTPUT_PATH ../bin)

#调试
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#输出可执行文件
add_executable(pool_multithread ${SRC})

#连接动态库
target_link_libraries(pool_multithread jsoncpp)
target_link_libraries(pool_multithread libmysqlclient.so)
