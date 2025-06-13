#pragma once
#include <mysql/mysql.h>
#include <string>
using namespace std;
class MySql
{
public:
  MySql();
  ~MySql();
  bool connect();
  bool update(string sql);
  MYSQL_RES *query(string sql);
  MYSQL *getConnection();
private:
  MYSQL *_conn;
};