#include <muduo/base/Logging.h>
#include <vector>

#include "chatservice.hpp"
#include "public.hpp"

using namespace std;
using namespace muduo;
using namespace placeholders;
ChatService *ChatService::instance()
{
  static ChatService service;
  return &service;
}
// 注册消息以及对应的Handler回调操作
ChatService::ChatService()
{
  // 用户基本业务管理相关事件处理回调注册
  _mesHandlerMap.insert({LOGIN_MSG,bind(&ChatService::Login,this,_1,_2,_3)});
  _mesHandlerMap.insert({LOGINOUT_MSG,bind(&ChatService::loginout,this,_1,_2,_3)});
  _mesHandlerMap.insert({REG_MSG,bind(&ChatService::reg,this,_1,_2,_3)});
  _mesHandlerMap.insert({ONE_CHAT_MSG,bind(&ChatService::oneChat,this,_1,_2,_3)});
  _mesHandlerMap.insert({ADD_FRIEND_MSG,bind(&ChatService::addFriend,this,_1,_2,_3)});

// 群组业务管理相关事件处理回调注册
  _mesHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
  _mesHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
  _mesHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
}
// 处理登录业务
void ChatService::Login(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
  int id=js["id"].get<int>();
  string pwd=js["password"];
  User user=_usermodel.query(id);
  if(user.getId()==id&&user.getPwd()==pwd)
  {
    if(user.getState()=="online")
    {
      json respone;
      respone["megid"]=LOGIN_MSG_ACK;
      respone["errno"]=2;
      respone["errmsg"]="this account is using,input another!";
    }else{
      // 登录成功，记录用户连接信息
      {
        lock_guard<mutex> lock(_connMutex);
        _userConnMap.insert({id,conn});
      }
      // 登录成功，更新用户状态信息 state offline=>online
      user.setState("online");
      _usermodel.updateState(user);

      json respon;
      respon["msgid"]=LOGIN_MSG_ACK;
      respon["errno"]=0;
      respon["id"]=user.getId();
      respon["name"]=user.getName();

      vector<string> vec=_offlineMsgmodel.query(id);
      if(!vec.empty())
      {
        respon["offlinemsg"]=vec;
        _offlineMsgmodel.remove(id);
      }
      // 查询该用户的好友信息并返回
      vector<User> uservec=_friendmodel.query(id);
      if(!uservec.empty())
      {
        vector<string> vec2;
        for(User &user:uservec)
        {
          json js;
          js["id"]=user.getId();
          js["name"]=user.getName();
          js["state"]=user.getState();
          vec2.push_back(js.dump());
        }
        respon["friend"]=vec2;
      }

      // 查询用户的群组信息
      vector<Group> groupuserVec = _groupmodel.queryGroups(id);
      if (!groupuserVec.empty())
      {
          // group:[{groupid:[xxx, xxx, xxx, xxx]}]
          vector<string> groupV;
          for (Group &group : groupuserVec)
          {
              json grpjson;
              grpjson["id"] = group.getId();
              grpjson["groupname"] = group.getName();
              grpjson["groupdesc"] = group.getDesc();
              vector<string> userV;
              for (GroupUser &user : group.getUsers())
              {
                  json js;
                  js["id"] = user.getId();
                  js["name"] = user.getName();
                  js["state"] = user.getState();
                  js["role"] = user.getRole();
                  userV.push_back(js.dump());
              }
              grpjson["users"] = userV;
              groupV.push_back(grpjson.dump());
            }
          respon["groups"] = groupV;
      }
      conn->send(respon.dump());
    }
  }else{
    // 该用户不存在，用户存在但是密码错误，登录失败
    json respon;
    respon["msgid"]=LOGIN_MSG_ACK;
    respon["errno"]=1;
    respon["errmsg"]="id or password is invalid!";
    conn->send(respon.dump());
  }
}
// 处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
  string name=js["name"];
  string pwd=js["password"];
  User user;
  user.setName(name);
  user.setPwd(pwd);
  bool state=_usermodel.insert(user);
  if(state)
  {
    json respon;
    respon["msgid"]=LOGIN_MSG_ACK;
    respon["errno"]=0;
    respon["id"]=user.getId();
    conn->send(respon.dump());
  }else{
    json js;
    js["msgid"]=LOGIN_MSG_ACK;
    js["errno"]=1;
    conn->send(js.dump());
  }
}
// 一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
  int toid=js["toid"].get<int>();
  {
    lock_guard<mutex> lock(_connMutex);
    auto it=_userConnMap.find(toid);
    if(it!=_userConnMap.end())
    {
      // toid在线，转发消息   服务器主动推送消息给toid用户
      it->second->send(js.dump());
      return;
    }
  }
  _offlineMsgmodel.insert(toid,js.dump());
}
// 添加好友业务
void ChatService::addFriend(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
  int userid=js["id"].get<int>();
  int friendid=js["friendid"].get<int>();
  _friendmodel.insert(userid,friendid);
}
// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
  int userid=js["id"].get<int>();
  string name=js["groupname"];
  string desc=js["groupdesc"];

  Group group(-1,name,desc);
  if(_groupmodel.createGroup(group))
  {
    _groupmodel.addGroup(userid,group.getId(),"creator");
  }
}
// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
  int userid = js["id"].get<int>();
  int groupid = js["groupid"].get<int>();
  _groupmodel.addGroup(userid, groupid, "normal");
}
// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
  int userid=js["id"].get<int>();
  int groupid=js["groupid"].get<int>();
  vector<int> userVec=_groupmodel.queryGroupUsers(userid,groupid);
  lock_guard<mutex> lock(_connMutex);
  for(int id:userVec)
  {
    auto it=_userConnMap.find(id);
    if(it!=_userConnMap.end())
    {
      it->second->send(js.dump());
    }else{
      _offlineMsgmodel.insert(id,js.dump());
    }
  }
}
// 处理注销业务
void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
  int userid=js["id"].get<int>();
  {
    lock_guard<mutex> lock(_connMutex);
    auto it=_userConnMap.find(userid);
    if(it!=_userConnMap.end())
    {
      _userConnMap.erase(it);
    }
  }
  User user(userid,"","","offline");
  _usermodel.updateState(user);
}
// 处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
  User user;
  {
    lock_guard<mutex> lock(_connMutex);
    for(auto it=_userConnMap.begin();it!=_userConnMap.end();++it)
    {
      if(it->second==conn)
      {
        // 从map表删除用户的链接信息
        user.setId(it->first);
        _userConnMap.erase(it);
        break;
      }
    }
  }
  if(user.getId()!=-1)
  {
    user.setState("offline");
    _usermodel.updateState(user);
  }
}
// 服务器异常，业务重置方法
void ChatService::reset()
{
  // 把online状态的用户，设置成offline
  _usermodel.resetState();
}
// 获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid)
{
  // 记录错误日志，msgid没有对应的事件处理回调
  auto it=_mesHandlerMap.find(msgid);
  if(it==_mesHandlerMap.end())
  {
    // 返回一个默认的处理器，空操作
    return [=](const TcpConnectionPtr &conn, json &js, Timestamp){
      LOG_ERROR << "msgid:" << msgid << " can not find handler!";
    };
  }else{
    return _mesHandlerMap[msgid];
  }
}