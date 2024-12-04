cmake_minimum_required(VERSION 3.0)
project(mprpc)

set(SRC_LIST userservice ../user.pb.cc)
add_executable(provider ${SRC_LIST})
target_link_libraries(provider mprpc protobuf)
