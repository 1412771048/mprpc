# 设置cmake的最低版本和项目名称
cmake_minimum_required(VERSION 3.22.1)
project(rpc)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib) #静态库生成位置
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib) #动态库生成位置
include_directories(${PROJECT_SOURCE_DIR}/src/include ${PROJECT_SOURCE_DIR}/example ${PROJECT_SOURCE_DIR}/thirdparty)
link_directories(${PROJECT_SOURCE_DIR}/lib)

# src包含了rpc框架所有的相关代码
add_subdirectory(src)
# example包含了rpc框架使用的示例代码
add_subdirectory(example)