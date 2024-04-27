cmake_minimum_required(VERSION 2.8)
project(mprpc)  #每个相对的顶级cmake都要写一个，意思就是最顶级的要写一个，下面每个模块的顶级cmake也要写一个
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address -fno-omit-frame-pointer -std=c++20") #debug模式，且能检查内存情况,不写此行就是release
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++20") 
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/bin) #程序生成位置, PROJECT_SOURCE_DIR是当前项目的顶级路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/lib)  #库生成位置
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/proto
    ${PROJECT_SOURCE_DIR}/thirdparty  #muduo库要用boost库的头文件
    ${PROJECT_SOURCE_DIR}/thirdparty/muduo/include
    ${PROJECT_SOURCE_DIR}/thirdparty/protobuf/include
    ${PROJECT_SOURCE_DIR}/thirdparty/simpleini
    ${PROJECT_SOURCE_DIR}/thirdparty/zookeeper/include

) 
link_directories(
    ${PROJECT_SOURCE_DIR}/lib #mprpc.a
    ${PROJECT_SOURCE_DIR}/thirdparty/muduo/lib
    ${PROJECT_SOURCE_DIR}/thirdparty/protobuf/lib
    ${PROJECT_SOURCE_DIR}/thirdparty/zookeeper/lib
) 

add_subdirectory(src)
add_subdirectory(example)