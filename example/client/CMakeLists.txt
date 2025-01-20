project(client)

file(GLOB_RECURSE SRC
    ./*.cpp 
    ../user.pb.cc
) #rpcprovider.h是库的头文件，不用包含源文件

add_executable(${PROJECT_NAME} ${SRC})

#头文件
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/example
    ${PROJECT_SOURCE_DIR}/thirdparty/mysqlclient
    ${PROJECT_SOURCE_DIR}/include
) 

#库位置
target_link_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/output
) 

#库名
target_link_libraries(${PROJECT_NAME} mprpc mysqlclient)