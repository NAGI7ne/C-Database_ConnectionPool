#include "MysqlConn.h"
#include "ConnectionPool.h"

using namespace std;
using namespace chrono;

void op1(int begin, int end) {   //不使用连接池

	for (int i = begin; i < end; i++) {
		MysqlConn conn;
		conn.connect("root", "miku39", "test", "192.168.160.129");
		char sql[1024] = { 0 };
		sprintf(sql, "insert into test values(%d, 18, 'woman', 'hachi')", i);
		conn.update(sql);
	}
}

void op2(ConnectionPool* pool, int begin, int end) {   //使用连接池

	for (int i = begin; i < end; i++) {
		shared_ptr<MysqlConn> conn = pool->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into test values(%d, 18, 'woman', 'hachi')", i);
		conn->update(sql);
	}
}

void test1() {   //单线程

#if 1
	//非连接池，单线程，用时：34200382800 纳秒, 34200毫秒
	steady_clock::time_point begin = steady_clock::now();
	op1(0, 5000);
	steady_clock::time_point end = steady_clock::now();
	auto length = end - begin;

	cout << "非连接池，单线程，用时：" << length.count() << " 纳秒, "
		<< length.count() / 1000000 << "毫秒" << endl;

#else
	//连接池，单线程，用时：6165706600 纳秒, 6165毫秒
	ConnectionPool* pool = ConnectionPool::getConnectPool();

	steady_clock::time_point begin = steady_clock::now();
	op2(pool, 0, 5000);
	steady_clock::time_point end = steady_clock::now();
	auto length = end - begin;

	cout << "连接池，单线程，用时：" << length.count() << " 纳秒, "
		<< length.count() / 1000000 << "毫秒" << endl;

#endif
}	

void test2() {   //多线程

#if 0
	//非连接池，多线程，用时：11846121900 纳秒, 11846毫秒
	MysqlConn conn;
	conn.connect("root", "miku39", "test", "192.168.160.129");
	steady_clock::time_point begin = steady_clock::now();
	thread t1(op1, 0, 1000);
	thread t2(op1, 1000, 2000);
	thread t3(op1, 2000, 3000);
	thread t4(op1, 3000, 4000);
	thread t5(op1, 4000, 5000);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	steady_clock::time_point end = steady_clock::now();
	auto length = end - begin;

	cout << "非连接池，多线程，用时：" << length.count() << " 纳秒, "
		<< length.count() / 1000000 << "毫秒" << endl;

#else
	//连接池，多线程，用时：2880769600 纳秒, 2880毫秒
	ConnectionPool* pool = ConnectionPool::getConnectPool();
	steady_clock::time_point begin = steady_clock::now();
	thread t1(op2, pool, 0, 1000);
	thread t2(op2, pool, 1000, 2000);
	thread t3(op2, pool, 2000, 3000);
	thread t4(op2, pool, 3000, 4000);
	thread t5(op2, pool, 4000, 5000);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	steady_clock::time_point end = steady_clock::now();
	auto length = end - begin;

	cout << "连接池，多线程，用时：" << length.count() << " 纳秒, "
		<< length.count() / 1000000 << "毫秒" << endl;

#endif


}

void query() {
	MysqlConn conn;
	conn.connect("root", "miku39", "test", "192.168.160.129");
	string sql = "insert into test values(3, 18, 'woman', 'hachi')";
	bool flag = conn.update(sql);
	cout << "flag vlue: " << flag << endl;

	sql = "select * from test";
	conn.query(sql);
	while (conn.next()) {
		cout << conn.value(0) << "," << conn.value(1) << ","
			<< conn.value(2) << "," << conn.value(3) << endl;
	}

	return ;
}

int main() {

	test1();

	return 0;
}