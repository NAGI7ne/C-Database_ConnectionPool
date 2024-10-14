# C++ Database Connection Pool

## 项目简介
    该项目是基于 C++ 实现的数据库连接池，用户可以复用数据库连接，从而提高系统的性能。项目依赖于 MySQL 库和 JSON 解析库。项目使用 `CMake` 构建，并在 Linux 环境下运行。

## 环境依赖
    在运行此项目之前，请确保已经在系统上安装了以下依赖：

1. **MySQL Client**
   - 该项目使用 MySQL 数据库，并且要求 MySQL 支持**远程连接**。

   安装命令：
   ```bash
   sudo apt-get install mysql-client libmysqlclient-dev

2. **JSON for Modern C++**
    - 用于解析 JSON 配置文件（例如数据库连接参数）。项目使用 jsoncpp 解析库来处理 JSON 文件。

    安装命令：
    ```bash
    sudo apt-get install libjsoncpp-dev

## 编译

    # 进入build目录，执行以下命令
    cmake ..
    make

## 运行
    
    # 可执行文件，位于bin/