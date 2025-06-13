#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <functional>
#include <string>

using namespace std;
using namespace placeholders;
using json=nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,const InetAddress &listenaddr,const string &nameArg)
  :_server(loop,listenaddr,nameArg),_loop(loop)
{
  _server.setConnectionCallback(bind(&ChatServer::OnConnection,this,_1));
  _server.setMessageCallback(bind(&ChatServer::onMessage,this,_1,_2,_3));
  _server.setThreadNum(4);
}
void ChatServer::start()
{
  _server.start();

}
void ChatServer::OnConnection(const TcpConnectionPtr &conn)
{
  if(!conn->connected())
  {
    conn->shutdown();
  }
}
void ChatServer::onMessage(const TcpConnectionPtr &conn,Buffer *buffer,Timestamp time)
{
  string buf=buffer->retrieveAllAsString();
  cout<<buf<<endl;
  //反序列化
  json js=json::parse(buf);
  auto msgHandler=ChatService::instance()->getHandler(js["msgid"].get<int>());
  msgHandler(conn,js,time);
}