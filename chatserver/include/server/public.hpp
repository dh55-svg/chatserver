#ifndef PUBLIC_H
#define PUBLIC_H
enum EnMsgType
{
  LOGIN_MSG=1,//登录消息
  LOGIN_MSG_ACK,//登录响应消息
  LOGINOUT_MSG,//注销消息
  REG_MSG,//注册消息
  REG_MSG_ACK,//注册响应消息
};

#endif // !PUBLIC_H
