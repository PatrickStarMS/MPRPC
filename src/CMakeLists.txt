aux_source_directory(. SRC_LIST)


# add_library(mprpc SHARED ${SRC_LIST}) #动态库
#由于muduo是静态库，所以mprpc也变成静态库就行
add_library(mprpc ${SRC_LIST})

target_link_libraries(mprpc muduo_net muduo_base zookeeper_mt)