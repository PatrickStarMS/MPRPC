#include "test.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;

// int main1()
// {
//   //封装了login请求对象的数据
//   LoginRequest req;
//   req.set_name("zhang san");
//   req.set_pwd("123456");

// //对象序列化=》char *
//   std::string send_str;
//   if(req.SerializeToString(&send_str))
//   {
//     std::cout << send_str << std::endl;
//   }


// //从send_str反序列化一个login请求对象
//   LoginRequest reqB;
//   if(reqB.ParseFromString(send_str))
//   {
//     std::cout << reqB.name() << std::endl;
//     std::cout << reqB.pwd() << std::endl;
//   }

//   return 0;
// }

int main()
{
  // LoginResponse rsp;
  // ResultCode *rc = rsp.mutable_result();
  // rc->set_errcode(1);
  // rc->set_errmsg("登陆失败");
  //序列化和反序列化都一样

  GetFriendListResponse rsp;
  ResultCode *rc = rsp.mutable_result();
  rc->set_errcode(0);

  User *user1 = rsp.add_friendlist();
  user1->set_name("zahangs");
  user1->set_age(20);
  user1->set_sex(User::MAN);
  std::cout << rsp.friendlist_size() << std::endl;
  return 0;
}