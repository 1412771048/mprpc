project(mprpc)

file(GLOB_RECURSE src 
    ./*.cc 
    ../thirdparty/simpleini/*.cc 
)

add_library(${PROJECT_NAME} SHARED ${src}) 

#头文件，PUBLIC实现依赖传递(当其他模块引用此模块时)，这样每个模块只需要看自己用了哪些头文件即可
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/thirdparty
    ${CMAKE_SOURCE_DIR}/thirdparty/muduo/include
    ${CMAKE_SOURCE_DIR}/thirdparty/protobuf-3.21.12/include
    ${CMAKE_SOURCE_DIR}/thirdparty/simpleini
    ${CMAKE_SOURCE_DIR}/thirdparty/zookeeper-3.4.9/include
)

#用到的库的位置
target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/muduo/lib
    ${CMAKE_SOURCE_DIR}/thirdparty/protobuf-3.21.12/lib
    ${CMAKE_SOURCE_DIR}/thirdparty/zookeeper-3.4.9/lib
)

#用到的库名
target_link_libraries(${PROJECT_NAME} PRIVATE muduo_net muduo_base pthread protobuf zookeeper_mt) #多线程的zookeeper库


