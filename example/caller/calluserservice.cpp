#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"
int main(int argc, char** argv) {

    //先进行框架初始化，初始化一次就够了
    MprpcApplication::Init(argc,argv);
    //使用者使用XXX_stub，来调用XXX服务
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    //rpc方法的请求参数
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    //rpc方法的回应
    fixbug::LoginResponse response;
    //发起rpc方法的调用  同步的rpc调用过程RpcChannel::CallMethod
    MprpcController mprpcController;
    stub.Login(
        &mprpcController, &request, &response,
        nullptr);  // RpcChannel->RpcChannel::CallMethod
                   // ,因此在CallMethod中集中来做所有rpc方法调用的参数序列化和网络发送
    //一次rpc调用完成，读调用的结果
    if(mprpcController.Failed())
    {
      std::cout << mprpcController.ErrorText() << std::endl;
    }
    else
    {
        // 一次rpc调用完成，读响应结果,响应的方法都是根据proto文件生成的 
        if(response.result().errcode()==0)
        {
        std::cout << "rpc login response success:" << response.success() << std::endl;

        } 
        else 
        {
            std::cout << "rpc login response error:" << response.result().errmsg() << std::endl;
        }
    }
    
    return 0;
}
