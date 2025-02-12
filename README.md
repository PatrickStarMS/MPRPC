# MPRPC 项目依赖与安装指南

## 1. muduo 安装


```bash
# 安装所需依赖
$ sudo apt install g++ cmake make libboost-dev

# 克隆 muduo 仓库
$ git clone -b cpp11 git@github.com:chenshuo/muduo.git
$ cd muduo

# 修改 CMake 配置，注释掉不需要编译的样例部分
$ vim CMakeLists.txt
# option(MUDUO_BUILD_EXAMPLES "Build Muduo examples" ON) 注释该行，不编译样例

# 编译 muduo
$ ./build.sh

# 安装 muduo
$ ./build.sh install  # muduo 编译后头文件和库文件都不在系统路径下

# 将 muduo 头文件拷贝到 mprpc 的 include 目录中
$ cd build/release-install-cpp11/include
$ cp -r muduo [your path]/mprpc/include/

# 将静态库文件拷贝到 mprpc 的 thirdparty 目录中
$ cd ../lib
$ cp libmuduo_base.a [your path]/mprpc/thirdparty/
$ cp libmuduo_net.a [your path]/mprpc/thirdparty/
```

## 2. protobuf 安装
```bash
# 安装编译 Protobuf 所需的依赖
$ sudo apt-get install autoconf automake libtool curl make g++

# 克隆 protobuf 仓库
$ git clone git@github.com:protocolbuffers/protobuf.git
$ cd protobuf

# 自动生成配置文件
$ ./autogen.sh

# 配置并编译 Protobuf
$ ./configure
$ make -j32

# 安装 Protobuf
$ sudo make install
$ sudo ldconfig  # 刷新系统可用动态库
```


## 3.Zookeeper安装
```bash
# 安装 Java 运行环境（Zookeeper 依赖）
$ sudo apt-get install default-jre

# 克隆 Zookeeper 仓库
$ git clone git@github.com:apache/zookeeper.git
$ cd zookeeper/conf

# 复制并修改 Zookeeper 配置文件
$ cp zoo_sample.cfg zoo.cfg
$ vim zoo.cfg  # 更改 dataDir=[你的存储路径]

# 编译并安装 Zookeeper
$ cd ../bin
$ ./zkServer.sh start  # 启动 Zookeeper 服务器

# 安装 Zookeeper 客户端
$ cd zookeeper/zookeeper-client/zookeeper-client-c
$ sudo ./configure
$ sudo make -j32
$ sudo make install
```
## 4.编译
```bash
# 克隆 MPRPC 仓库
$ git clone git@github.com:shuming1998/mprpc.git
$ cd mprpc

# 给予构建脚本执行权限
$ chmod +x autoBuild.sh

# 给予构建脚本执行权限
$ chmod +x autoBuild.sh

# 执行构建
$ sudo ./autobuild.sh

# 启动 RPC 服务提供方
$ cd bin
$ ./provider -i 配置文件(服务器和Zookeeper的ip+port)

# 启动 RPC 服务调用方
$ ./consumer -i 配置文件(服务器和Zookeeper的ip+port)

```
