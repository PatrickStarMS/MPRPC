cmake_minimum_required(VERSION 3.0)
project(mprpc)

set(SRC_LIST calluserservice ../user.pb.cc)
add_executable(consumer ${SRC_LIST})
target_link_libraries(consumer mprpc protobuf)
