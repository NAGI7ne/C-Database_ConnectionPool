#pragma once
#include<iostream>
#include<mysql.h>
#include<chrono>
#include<mutex>

class MysqlConn
{
public:
	//初始化连接
	MysqlConn();
	//释放连接
	~MysqlConn();
	//连接数据库
	bool connect(std::string name, std::string passwd, std::string dbname, std::string ip, unsigned int port = 3306);
	//操作数据库 :update insert, delete
	bool update(std::string sql);
	//查询数据库
	bool query(std::string sql);
	//遍历查询结果集
	bool next();
	//得到结果集的字段值
	std::string value(int index);
	//事务操作
	bool transaction();
	//事务回滚
	bool rollback();
	//事务提交
	bool commit();
	//刷新起始的空闲时间点
	void refreshAliveTime();
	//计算连接的存活时长
	long long getAliveTime();

private:
	void freeResult();  //用于释放结果集
	MYSQL* m_conn = nullptr;
	MYSQL_RES* m_result = nullptr;
	MYSQL_ROW m_row = nullptr;
	std::chrono::steady_clock::time_point m_alivetime;

};

