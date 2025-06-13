#pragma once
#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>
using namespace std;
using namespace muduo;
using namespace muduo::net;


#include "json.hpp"
#include "usermodel.hpp"
#include "groupmodel.hpp"
#include "friendmodel.hpp"
#include "offlinemessagemodel.hpp"

using json=nlohmann::json;
// 表示处理消息的事件回调方法类型
using MsgHandler=function<void(const TcpConnectionPtr &conn,json &js,Timestamp)>;
class ChatService
{
public:
  static ChatService *instance();
  // 处理登录业务
  void Login(const TcpConnectionPtr &conn,json &js,Timestamp time);
  // 处理注册业务
  void reg(const TcpConnectionPtr &conn,json &js,Timestamp time);
  // 一对一聊天业务
  void oneChat(const TcpConnectionPtr &conn,json &js,Timestamp time);
  // 添加好友业务
  void addFriend(const TcpConnectionPtr &conn,json &js,Timestamp time);
  // 创建群组业务
  void createGroup(const TcpConnectionPtr &conn,json &js,Timestamp time);
  // 加入群组业务
  void addGroup(const TcpConnectionPtr &conn,json &js,Timestamp time);
  // 群组聊天业务
  void groupChat(const TcpConnectionPtr &conn,json &js,Timestamp time);
  // 处理注销业务
  void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);
  // 处理客户端异常退出
  void clientCloseException(const TcpConnectionPtr &conn);
  // 服务器异常，业务重置方法
  void reset();
  // 获取消息对应的处理器
  MsgHandler getHandler(int msgid);
  // 从redis消息队列中获取订阅的消息
  void handleRedisSubscribeMessage(int, string);
private:
  ChatService();
  // 存储消息id和其对应的业务处理方法
  unordered_map<int,MsgHandler> _mesHandlerMap;
  // 存储在线用户的通信连接
  unordered_map<int,TcpConnectionPtr> _userConnMap;
  // 定义互斥锁，保证_userConnMap的线程安全
  mutex _connMutex;
  // 数据操作类对象
  UserModel _usermodel;
  OfflineMsgModel _offlineMsgmodel;
  FriendModel _friendmodel;
  GroupModel _groupmodel;


};