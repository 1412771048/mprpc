file(GLOB_RECURSE src 
    ./*.cc 
    ../thirdparty/simpleini/*.cc 
)
add_library(mprpc ${src}) #muduo库是静态库，这里只能生成静态库
target_link_libraries(mprpc muduo_net muduo_base pthread)