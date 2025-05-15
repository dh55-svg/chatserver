#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <functional>
#include <string>
using namespace std;
using namespace placeholders;
using json=nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,const InetAddress &listenAddr,const string &nameArg):
  server_(loop,listenAddr,nameArg),loop_(loop)
{

}
void ChatServer::start()
{
  server_.start();
}
void ChatServer::onConnect(const TcpConnectionPtr& conn)
{
  if(!conn->connected())
  {
    conn->shutdown();
  }
}
void ChatServer::onMessage(const TcpConnectionPtr& conn,Buffer *buffer,Timestamp time)
{
  string buf=buffer->retrieveAllAsString();
  cout<<buf<<endl;
  json js=json::parse(buf);
  auto msgHandler=ChatService::instance()->getHandler(js["msgid"].get<int>());
  msgHandler(conn,js,time);
}