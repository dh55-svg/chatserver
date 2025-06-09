#include "chatserver.hpp"
#include "json.hpp"

#include <iostream>
#include <functional>
#include <string>

using namespace std;
using namespace placeholders;
using json=nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,const InetAddress &listenaddr,const string &nameArg)
  :_server(loop,listenaddr,nameArg),_loop(loop)
{
  
}