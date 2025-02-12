project(server)

file(GLOB_RECURSE SRC 
    ./*.cpp
   ../user.pb.cc
)

add_executable(${PROJECT_NAME} ${SRC})

#头文件
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/example
    ${CMAKE_SOURCE_DIR}/thirdparty/mysqlclient
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/thirdparty/protobuf-3.21.12/include
) 

#库位置
target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/output
    ${CMAKE_SOURCE_DIR}/thirdparty/protobuf-3.21.12/lib
) 

#库名
target_link_libraries(${PROJECT_NAME} PRIVATE mprpc mysqlclient protobuf)