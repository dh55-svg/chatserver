#ifndef CHATSERVICE_H
#define CHATSERVICE_H
#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>
using namespace std;
using namespace muduo;
using namespace muduo::net;

#include "json.hpp"
#include "usermodel.hpp"
using json=nlohmann::json;
//表示处理消息的时间回调方法类型
using MsgHandler=function<void(const TcpConnectionPtr &conn,json &js,Timestamp ts)>;
class ChatService
{
public:
  static ChatService* instance();
  void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
  void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);
  void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);
  MsgHandler getHandler(int msgid);
private:
  ChatService();
  //储存消息id和其对应的业务处理方法
  unordered_map<int,MsgHandler> _msgHandlerMap;
  //存储在线用户的通信连接
  unordered_map<int,TcpConnectionPtr> _userConnMap;
  std::mutex _connMutex;//定义互斥锁，保护_userConnMap安全
  UserModel _userModel;
};
#endif // !CHATSERVICE_H
