#pragma
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
  //获取连接
  MYSQL *getConnection();
private:
  MYSQL *conn_;
};