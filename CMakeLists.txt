# cmake最低版本
cmake_minimum_required(VERSION 3.28)
project(CPP_dbConnectionPool)

#C++ 20
set(CMAKE_CXX_STANDARD 20)

#源文件，头文件路径
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC)
include_directories(${PROJECT_SOURCE_DIR}/inc)

#输出可执行文件路径
set(EXECUTABLE_OUTPUT_PATH ../bin)

#输出可执行文件
add_executable(dbConnPool ${SRC})

#连接动态库
target_link_libraries(dbConnPool jsoncpp)
target_link_libraries(dbConnPool libmysqlclient.so)
