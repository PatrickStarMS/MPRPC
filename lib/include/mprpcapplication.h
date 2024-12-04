#ifndef MPRPCAPPLICATION_H
#define MPRPCAPPLICATION_H
#include "mprpcconfig.h"
//框架的基础类--单例模式,负责框架的初始化操作
class MprpcApplication
{
public:
 static void Init(int argc,char** argv);
 static MprpcApplication& GetInstance();
 static MprpcConfig& GetConfig();

private:
 
 MprpcApplication() {}
 MprpcApplication(const MprpcApplication&) = delete;
 MprpcApplication(MprpcApplication&&) = delete;

 //组合框架的各部分
 static MprpcConfig _mprpcConfig;
};

#endif 