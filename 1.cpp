

#include <iostream>
int main()
{
    
std::string src_buf = "rpcserverip";
std::string new_str = src_buf.substr(0, 0); // 返回空字符串
std::cout << "src_buf: " << src_buf << std::endl;  // 输出 "rpcserverip"
std::cout << "new_str: " << new_str << std::endl;  // 输出空字符串
}
