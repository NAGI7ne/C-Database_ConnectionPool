#pragma once
#include<queue>
#include<mutex>
#include <condition_variable>
#include "MysqlConn.h"
class ConnectionPool
{
public:
	static ConnectionPool* getConnectPool();    //采用单列模式
	ConnectionPool(const ConnectionPool& obj) = delete;
	ConnectionPool& operator=(const ConnectionPool& obj) = delete;
	std::shared_ptr<MysqlConn> getConnection();
	~ConnectionPool();


private:
	ConnectionPool();    
	bool parseJsonFile();     //加载配置文件
	void produceConnection(); //生产连接
	void recycleConnection(); //回收连接
	void addConnection();   //与数据库建立连接


	std::string m_ip;
	std::string m_username;
	std::string m_dbname;
	unsigned long m_port;
	std::string m_passwd;
	int m_maxsize;
	int m_minsize;
	int m_timeout;      //超时时长
	int m_maxIdleTime;   //最大空闲时长
	std::queue<MysqlConn*> m_connectionQ;   //连接池队列
	std::mutex m_mutex;  
	std::condition_variable p_cond, g_cond;

};

