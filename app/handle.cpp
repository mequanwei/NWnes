#include "app/handle.h"
#include "db/connectionpool.h"
#include <mysql/mysql.h>
#include <unistd.h>
using namespace std;



int Handle::process()
{

	ConnectionPool &pool = ConnectionPool::GetInstance();
	MYSQL *mysql = NULL;
	string sql;

	mysql = pool.GetConnection();
	sql = "insert into test (message) values(" + to_string(getpid()) + ")";

	if (mysql_query(mysql, sql.c_str()))
	{
		//error
	}

	return 0;
}
