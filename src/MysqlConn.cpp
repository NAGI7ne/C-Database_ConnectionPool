#include "MysqlConn.h"

MysqlConn::MysqlConn()
{
	m_conn = mysql_init(nullptr);
	mysql_set_character_set(m_conn, "utf8");

}

MysqlConn::~MysqlConn()
{
	if (m_conn != nullptr) {
		mysql_close(m_conn);
	}

	freeResult();
}

bool MysqlConn::connect(std::string name, std::string passwd, std::string dbname, std::string ip, unsigned int port)
{
	MYSQL* ptr = mysql_real_connect(m_conn, ip.c_str(), name.c_str(),
					passwd.c_str(), dbname.c_str(), port, nullptr, 0);

	if(ptr == nullptr)
		return false;

	return true;

	//return ptr != nullptr;
}

bool MysqlConn::update(std::string sql)
{
	if(mysql_query(m_conn, sql.c_str()))  //返回0成功，返回非零失败
		return false;   
	return true;
}

bool MysqlConn::query(std::string sql)
{
	freeResult();
	if (mysql_query(m_conn, sql.c_str()))
		return false;

	m_result = mysql_store_result(m_conn);  //保存查询到的结果集


	return false;
}

bool MysqlConn::next()
{
	if (m_result != nullptr) {
		m_row = mysql_fetch_row(m_result);  //读一次结果集
		if (m_row != nullptr)
			return true;
	}

	return false;
}

std::string MysqlConn::value(int index)
{
	int colcount = mysql_num_fields(m_result);  //得到结果集的列数
	if (index >= colcount || index < 0)
		return std::string();

	char* val = m_row[index]; 
	unsigned long lenth = mysql_fetch_lengths(m_result)[index]; //得到一个字符值的长度
														//防止在string转换的时候丢失数据

	return std::string(val, lenth); //把lenth长度的char* 转为string
}

bool MysqlConn::transaction()
{
	return mysql_autocommit(m_conn, false); //设置事务提交为手动提交
}

bool MysqlConn::rollback()
{

	return mysql_rollback(m_conn);
}

bool MysqlConn::commit()
{
	return mysql_commit(m_conn);
}

void MysqlConn::refreshAliveTime()
{
	m_alivetime = std::chrono::steady_clock::now();
}

long long MysqlConn::getAliveTime()
{
	std::chrono::nanoseconds nanores = std::chrono::steady_clock::now() - m_alivetime;
	std::chrono::milliseconds millires = std::chrono::duration_cast<std::chrono::milliseconds> (nanores);
	return millires.count();
}

void MysqlConn::freeResult()
{
	if (m_result != nullptr)
		mysql_free_result(m_result);
}



