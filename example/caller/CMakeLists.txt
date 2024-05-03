file(GLOB_RECURSE src 
    ./*.cc 
    ../user.pb.cc
) #rpcprovider.h是库的头文件，不用包含源文件
add_executable(consumer ${src})
target_link_libraries(consumer mprpc protobuf)