#include "usermodel.hpp"
#include "db.hpp"
#include <iostream>
using namespace std;
bool UserModel::insert(User &user)
{
  char sql[1024]={0};
  sprintf(sql,"insert into user(name,password,state) values('%s','%s','%s')",
      user.getName().c_str(),user.getPwd().c_str(),user.getState().c_str());
  MySql mysql;
  if(mysql.connect())
  {
    if(mysql.update(sql))
    {
      user.setId(mysql_insert_id(mysql.getConnection()));
      return true;
    }
  }
  return false;
}
User UserModel::query(int id)
{
  char sql[1024]={0};
  sprintf(sql,"select * from user where id = %d",id);
  MySql mysql;
  if(mysql.connect())
  {
    MYSQL_RES *res=mysql.query(sql);
    if(res!=nullptr)
    {
      MYSQL_ROW row=mysql_fetch_row(res);
      if(row!=nullptr)
      {
        User user;
        user.setId(atoi(row[0]));
        user.setName(row[1]);
        user.setPwd(row[2]);
        user.setState(row[3]);
        mysql_free_result(res);
        return user;
      }
    }
  }
  return User();
}
// 更新用户的状态信息
bool UserModel::updateState(User user)
{
  char sql[1024]={0};
  sprintf(sql,"update user set state='%s' where id ='%d'",user.getState().c_str(),user.getId());
  MySql mysql;
  if(mysql.connect())
  {
    if(mysql.update(sql))
    {
      return true;
    }
  }
  return false;
}
// 重置用户的状态信息
void UserModel::resetState()
{
  char sql[1024]={0};
  sprintf(sql,"update user set state='offline' where state='online'");

  MySql mysql;
  if(mysql.connect())
  {
    mysql.update(sql);
  }
}