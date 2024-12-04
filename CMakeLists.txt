cmake_minimum_required(VERSION 3.0)
project(mprpc)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_BUILD_TYPE "Debug")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)

link_directories(${PROJECT_SOURCE_DIR}/lib)

# src包含了mprpc框架的所有相关代码
add_subdirectory(src)
#example包含了mprpc框架使用的示例代码
add_subdirectory(example)