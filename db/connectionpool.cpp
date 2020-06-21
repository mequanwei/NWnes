#include "db/connectionpool.h"
#include "sem/sem.h"
#include "sem/mutex.h"
#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>

using namespace std;


ConnectionPool::ConnectionPool()
{
	m_CurConn = 0;
	m_FreeConn = 0;
	m_MaxConn = 0;

}

ConnectionPool& ConnectionPool::GetInstance()
{
	static ConnectionPool pool_;
	return pool_;
}

//构造初始化
void ConnectionPool::init(string url, string User, string PassWord, string DBName, int Port, int MaxConn, int close_log)
{
	m_url = url;
	m_Port = Port;
	m_User = User;
	m_PassWord = PassWord;
	m_DatabaseName = DBName;
//	m_close_log = close_log;

	for (int i = 0; i < MaxConn; i++)
	{
		MYSQL *con = NULL;
		con = mysql_init(con);

		if (con == NULL)
		{
//			LOG_ERROR("MySQL Error");
			exit(1);
		}
		con = mysql_real_connect(con, url.c_str(), User.c_str(), PassWord.c_str(), DBName.c_str(), Port, NULL, 0);

		if (con == NULL)
		{
//			LOG_ERROR("MySQL Error");
			exit(1);
		}
		connList.push_back(con);
		++m_FreeConn;
	}
	reserve = Sem(m_FreeConn);
	m_MaxConn = m_FreeConn;
}


//当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MYSQL *ConnectionPool::GetConnection()
{
	MYSQL *con = NULL;

	if (0 == connList.size())
		return NULL;

	reserve.wait();

	lock.Lock();

	con = connList.front();
	connList.pop_front();

	--m_FreeConn;
	++m_CurConn;

	lock.Unlock();
	return con;
}

//释放当前使用的连接
bool ConnectionPool::ReleaseConnection(MYSQL *con)
{
	if (NULL == con)
		return false;

	lock.Lock();

	connList.push_back(con);
	++m_FreeConn;
	--m_CurConn;

	lock.Unlock();

	reserve.post();
	return true;
}

//销毁数据库连接池
void ConnectionPool::DestroyPool()
{

	lock.Lock();
	if (connList.size() > 0)
	{
		list<MYSQL *>::iterator it;
		for (it = connList.begin(); it != connList.end(); ++it)
		{
			MYSQL *con = *it;
			mysql_close(con);
		}
		m_CurConn = 0;
		m_FreeConn = 0;
		connList.clear();
	}

	lock.Unlock();
}

//当前空闲的连接数
int ConnectionPool::GetFreeConn()
{
	return this->m_FreeConn;
}

ConnectionPool::~ConnectionPool()
{
	DestroyPool();
}


