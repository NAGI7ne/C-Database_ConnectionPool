#include "ConnectionPool.h"
#include <jsoncpp/json/json.h>
#include <fstream>
#include <thread>
#include <chrono>

ConnectionPool* ConnectionPool::getConnectPool()
{
	static ConnectionPool pool;
	return &pool;
}

std::shared_ptr<MysqlConn> ConnectionPool::getConnection()
{
	std::unique_lock<std::mutex> locker(m_mutex);
	while (m_connectionQ.empty()) {
		if(std::cv_status::timeout == g_cond.wait_for(locker, std::chrono::milliseconds(m_timeout)))
			if (m_connectionQ.empty())
				continue;
	}
	std::shared_ptr<MysqlConn> connptr(m_connectionQ.front(), [this](MysqlConn* conn) {
		std::lock_guard<std::mutex> locker(m_mutex);
		m_connectionQ.push(conn);
		conn->refreshAliveTime();
		});
	m_connectionQ.pop();
	p_cond.notify_one();
	return connptr;
}

ConnectionPool::~ConnectionPool()
{
	if (!m_connectionQ.empty()) {
		MysqlConn* conn = m_connectionQ.front();
		m_connectionQ.pop();
		delete conn;
	}
}

void ConnectionPool::produceConnection()
{
	while (1) {
		std::unique_lock<std::mutex> locker(m_mutex);
		while (m_connectionQ.size() >= m_minsize) {
			p_cond.wait(locker);
		}
		addConnection();
		g_cond.notify_one();
	}
}

void ConnectionPool::recycleConnection()
{
	while (1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		std::lock_guard<std::mutex> locker(m_mutex);
		while (m_connectionQ.size() > m_maxsize) {
			MysqlConn* conn = m_connectionQ.front();
			if (conn->getAliveTime() >= m_maxIdleTime) {
				m_connectionQ.pop();
				delete conn;
			}
			else {
				break;
			}
		}
	}
}

void ConnectionPool::addConnection()
{
	MysqlConn* conn = new MysqlConn;
	conn->connect(m_username, m_passwd, m_dbname, m_ip, m_port);
	conn->refreshAliveTime();
	m_connectionQ.push(conn);
}


ConnectionPool::ConnectionPool()
{
	if (!(parseJsonFile())) {
		return;
	}

	for (int i = 0; i < m_minsize; i++) {
		addConnection();
	}

	std::thread producer(&ConnectionPool::produceConnection, this);
	std::thread recycler(&ConnectionPool::recycleConnection, this);
	producer.detach();
	recycler.detach();

}

bool ConnectionPool::parseJsonFile()
{
	std::ifstream ifs("dbconf.json");
	Json::Reader rd;
	Json::Value root;
	rd.parse(ifs, root);
	if (root.isObject()) {
		m_ip = root["ip"].asString();
		m_port = root["port"].asInt();
		m_username = root["username"].asString();
		m_passwd = root["password"].asString();
		m_dbname = root["dbname"].asString();
		m_minsize = root["minsize"].asInt();
		m_maxsize = root["maxsize"].asInt();
		m_maxIdleTime = root["maxIdleTime"].asInt();
		m_timeout = root["timeout"].asInt();
		return true;
	}

	return false;
}


