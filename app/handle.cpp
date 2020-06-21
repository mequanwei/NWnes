#include "app/handle.h"

#include <vector>
#include "db/connectionpool.h"
#include <mysql/mysql.h>
using namespace std;
int mergeSort(vector<int> a)
{
return 0;

}



void handle(int pid)
{

	ConnectionPool &pool = ConnectionPool::GetInstance();
	pool.init("localhost","root","mysql","server",5508,4,0);
	MYSQL *mysql = NULL;
	mysql = pool.GetConnection();
	string sql;
	sql = "insert into test (message) values(" + to_string(pid) + ")";
	if (mysql_query(mysql, sql.c_str()))
	{
		//error
	}



}
