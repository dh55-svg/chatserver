#pragma once
#include "user.hpp"
#include <vector>
using namespace std;
// 维护好友信息的操作接口方法
class FriendModel
{
public:
  void insert(int userid,int friendid);
  // 返回用户好友列表
  vector<User> query(int userid);
};