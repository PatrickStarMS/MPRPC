#pragma once
#include <google/protobuf/service.h>
#include <string>

class MprpcController:public google::protobuf::RpcController
{
  public:
   MprpcController();
   //重写方法---都是修改下面的成员变量的
   void Reset();
   bool Failed() const;
   std::string ErrorText() const;
   void SetFailed(const std::string& reason);

   //未实现具体的功能
   void StartCancel();
   bool IsCanceled() const;
   void NotifyOnCancel(google::protobuf::Closure* callback);

  private:
   bool m_failed;//rpcd方法执行过程中的状态
   std::string m_errText;//rpc方法执行过程中的错误信息
};