#include "db.hpp"
#include <muduo/base/Logging.h>
static string server="127.0.0.1";
static string user="root";
static string password="123456";
static string dbname="chat";
MySql::MySql()
{
  _conn=mysql_init(nullptr);
}
MySql::~MySql()
{
  if(_conn!=nullptr)
    mysql_close(_conn);
}
bool MySql::connect()
{
  MYSQL *p=mysql_real_connect(_conn,server.c_str(),user.c_str(),
        password.c_str(),dbname.c_str(),3306,nullptr,0);
  if(p!=nullptr)
  {
    mysql_query(_conn,"set names gbk");
    LOG_INFO<<"connection mysql success";
  }
  else
  {
      LOG_INFO << "connect mysql fail!";
  }
  return p;
}
bool MySql::update(string sql)
{
  if(mysql_query(_conn,sql.c_str()))
  {
    LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                 << sql << "更新失败!";
    return false;
  }
  return true;
}
MYSQL_RES *MySql::query(string sql)
{
  if(mysql_query(_conn,sql.c_str()))
  {
    LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                 << sql << "查询失败!";
        return nullptr;
  }
  return mysql_use_result(_conn);
}
MYSQL *MySql::getConnection()
{
  return _conn;
}
