project(mprpc)

file(GLOB_RECURSE src 
    ./*.cc 
    ../thirdparty/simpleini/*.cc 
)

add_library(${PROJECT_NAME} SHARED ${src}) 

#头文件
target_include_directories(${PROJECT_NAME} PRIVATE     
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/thirdparty  #muduo库要用boost库的头文件
    ${PROJECT_SOURCE_DIR}/thirdparty/muduo/include
    ${PROJECT_SOURCE_DIR}/thirdparty/protobuf-3.21.12/include
    ${PROJECT_SOURCE_DIR}/thirdparty/simpleini
    ${PROJECT_SOURCE_DIR}/thirdparty/zookeeper-3.4.9/include
)

#库位置
target_link_directories(${PROJECT_NAME} PRIVATE 
    ${PROJECT_SOURCE_DIR}/output #mprpc.so
    ${PROJECT_SOURCE_DIR}/thirdparty/muduo/lib
    ${PROJECT_SOURCE_DIR}/thirdparty/protobuf-3.21.12/lib
    ${PROJECT_SOURCE_DIR}/thirdparty/zookeeper-3.4.9/lib
)

#库名
target_link_libraries(${PROJECT_NAME} muduo_net muduo_base pthread protobuf zookeeper_mt) #多线程的zookeeper库






