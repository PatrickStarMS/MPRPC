#include "mprpcconfig.h"
#include <iostream>
#include <string>

void MprpcConfig::Trim(std::string &src_buf)
{
  int idx = src_buf.find_first_not_of(' ');
    if(idx!=-1)
    {
      //说明字符串前面有空格
      src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }
    //去掉字符串后面多余的空格
    idx = src_buf.find_last_not_of(' ');
    if(idx!=-1)
    {
      //说明字符串后面有空格,这里是截取idx+1，因为这里找的是不为空格的，所以要包含该元素
      src_buf = src_buf.substr(0, idx+1);
    }
}

void MprpcConfig::loadConfig(const char* config_file)
{
  FILE *pf = fopen(config_file,"r");
  if(pf==nullptr)
  {
    std::cout <<config_file<< "is not exist!" << std::endl;
    exit(EXIT_FAILURE);
  }
  //1.注释  2.去掉开头多余的空格 3.正确的配置项=
  while(!feof(pf))
  {
    char buf[512];
    fgets(buf, 512, pf);//从哪个流读

    //去掉字符串前面多余的空格（c++，没有相关函数,自己实现）,转成string好实现
    std::string src_buf(buf);
    
    //判断#的注释或者全是空格
    if(src_buf[0]=='#'|| src_buf.empty())
    {
      continue;
    }
    Trim(src_buf);
    // 解析配置项按照=解析
    int idx = src_buf.find('=');
    if(idx==-1)
    {
      //配置不合法
      continue;
    }
    std::string key;
    std::string value;
    key = src_buf.substr(0, idx );
    //因为等号前后可能有空格
    Trim(key);
 
    //去掉可能存在的尾部换行符
    int endidx = src_buf.find('\n', idx);
    //这里找到换行符，这里是从idx+1开始截的，从idx+1开始数n各正好含有endidx，字符串中应该不包含，，因此-1
    //如果是从idx开始解，不含有endidx，就不需要-1了
    value = src_buf.substr(idx + 1, endidx - idx-1);
    Trim(value);
    m_configMap.insert({key, value});
  }
}

std::string MprpcConfig::Load(const std::string &key)
{
  //不要用中括号，中括号有副作用，要用find
  auto it = m_configMap.find(key);
  if(it!=m_configMap.end());
  { return it->second; }
  return "";
}