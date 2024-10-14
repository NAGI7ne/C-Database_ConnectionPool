#include "ConnectionPool.h"
#include <jsoncpp/json/json.h>
#include <fstream>
#include <thread>
#include <chrono>
using namespace Json;

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
	std::cout << "... this is parse ..." << std::endl;
	std::ifstream ifs("/home/nagine/itest2/ConnectionPool/src/dbconf.json");
	//std::ifstream ifs("dbconf.json");

	std::string line;
	// if (ifs.is_open()) {
    //     while (getline(ifs, line)) { // 按行读取
    //         std::cout << line << '\n';
    //     }
    //     ifs.close();
    // } else {
    //     std::cout << "Unable to open file" << '\n';
    // }
	
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
		std::cout << "... parse down ..." << std::endl;
		return true;
	}
	std::cout << "... parse false ..." << std::endl;
	return false;
}


