#顶级cmake文件，一个大型工程只有一个，在这里定义一些全局的设置
cmake_minimum_required(VERSION 3.15)
if (NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Release) 
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON) #必须支持指定的c++标准

set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output) #程序生成位置, CMAKE_SOURCE_DIR是整个工程的顶级路径
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)  #库生成位置
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address -fno-omit-frame-pointer")

#进入各个子目录，即每个人开发的模块
add_subdirectory(src)
add_subdirectory(example/server)
add_subdirectory(example/client)