file(GLOB_RECURSE src 
    ./*.cc 
    ../../proto/user.pb.cc
) #rpcprovider.h是库的头文件，不用包含源文件
add_executable(provider ${src})
target_link_libraries(provider mprpc protobuf)