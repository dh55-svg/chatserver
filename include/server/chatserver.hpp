#pragma once
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;
class ChatServer
{
public:
  ChatServer(EventLoop *loop,const InetAddress &listenaddr,const string &nameArg);
  void start();
private:
  void OnConnection(const TcpConnectionPtr &);
  void onMessage(const TcpConnectionPtr &,Buffer *,Timestamp);
  TcpServer _server;
  EventLoop *_loop;
};