#include "mprpccontroller.h"

MprpcController::MprpcController() { m_failed = false;
  m_errText = "";
}
// 重写方法
void MprpcController:: Reset()
{
    m_failed=false;
    m_errText = "";
}
bool MprpcController::Failed() const
{
    return m_failed;
}
std::string MprpcController::ErrorText() const
{
    return m_errText;
}
void MprpcController::SetFailed(const std::string& reason)
{
    m_failed=true;
    m_errText = reason;
}


void MprpcController:: StartCancel(){}
bool MprpcController::IsCanceled() const{}
void MprpcController::NotifyOnCancel(google::protobuf::Closure* callback){}