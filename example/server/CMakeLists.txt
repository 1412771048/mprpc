project(server)

file(GLOB_RECURSE SRC 
    ./*.cpp
   ../user.pb.cc
)

add_executable(${PROJECT_NAME} ${SRC})

#头文件，对于此模块，不写protobuf也行，因为用到了mprpc.so，依赖也传递给你了，但建议写上，更清晰的看本模块的依赖
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/example
    ${CMAKE_SOURCE_DIR}/thirdparty/mysqlclient
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/thirdparty/protobuf-3.21.12/include
) 

#库位置
target_link_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/output
    ${CMAKE_SOURCE_DIR}/thirdparty/protobuf-3.21.12/lib
) 

#库名
target_link_libraries(${PROJECT_NAME} mprpc mysqlclient protobuf)