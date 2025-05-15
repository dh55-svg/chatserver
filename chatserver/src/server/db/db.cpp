#include "db.hpp"
#include <muduo/base/Logging.h>

static string server="127.0.0.1";
static string user="root";
static string password="123456";
static string dbname="chat";

MySql::MySql()
{
  conn_=mysql_init(nullptr);

}
MySql::~MySql()
{
  if(conn_!=nullptr)
    mysql_close(conn_);
}
bool MySql::connect()
{
  MYSQL *p=mysql_real_connect(conn_,server.c_str(),user.c_str(),
                              password.c_str(),dbname.c_str(),3306,nullptr,0);
  if(p!=nullptr)
  {
    mysql_query(conn_,"set names gbk");
    LOG_INFO<<"connect mysql success!";
  }else
  {
    LOG_INFO << "connect mysql fail!";
  }
  return p;                          
}
bool MySql::update(string sql)
{
  if(mysql_query(conn_,sql.c_str()))
  {
    LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
    << sql << "更新失败!";
    return false;
  }
  return true;
}
MYSQL_RES *MySql::query(string sql)
{
  if (mysql_query(conn_, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                 << sql << "查询失败!";
        return nullptr;
    }
    return mysql_use_result(conn_);
    
}
MYSQL *MySql::getConnection()
{
  return conn_;
}