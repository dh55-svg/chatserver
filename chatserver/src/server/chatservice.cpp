#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <vector>
using namespace std;
using namespace muduo;
using namespace placeholders;
ChatService* ChatService::instance()
{
  static ChatService cs;
  return &cs;
}
ChatService::ChatService()
{
  _msgHandlerMap.insert({LOGIN_MSG,bind(&ChatService::login,this,_1,_2,_3)});
  _msgHandlerMap.insert({LOGINOUT_MSG,bind(&ChatService::loginout,this,_1,_2,_3)});
  _msgHandlerMap.insert({REG_MSG,bind(&ChatService::reg,this,_1,_2,_3)});

}
MsgHandler ChatService::getHandler(int msgid)
{
  auto it=_msgHandlerMap.find(msgid);
  if(it==_msgHandlerMap.end())
  {
    return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
    {
      LOG_ERROR << "msgid:" << msgid << " can not find handler!";
    };
  }else{
    return _msgHandlerMap[msgid];
  }
}
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
  int id=js["id"].get<int>();
  string password=js["password"];
  User user=_userModel.query(id);
  if(user.getId()==id&&user.getPwd()==password)
  {
    if(user.getState()=="online")
    {
      
    }
  }
}
