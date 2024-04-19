#include "databank.h"


std::shared_mutex DataBank::rw_mtx; 
std::unordered_map<std::string, std::string> DataBank::config_map;
std::unordered_map<std::string, google::protobuf::Service*> DataBank::service_map;


void* DataBank::Lock(const std::string& data, const LockMode& mode) {
    if (mode == READ) {
        rw_mtx.lock_shared(); //获取共享读锁
    } else if (mode == WRITE) {
        rw_mtx.lock(); //获取独占写锁
    } else {
        std::cerr << "LockMode errno!" << std::endl;
        exit(EXIT_FAILURE); //这属于代码错误，直接退出程序
    }

    if (data == "config_map") {
        return &config_map;
    } else if (data == "service_map") {
        return &service_map;
    } 

    return nullptr;
}   

void DataBank::Unlock(const std::string& data, const LockMode& mode) {
    if (mode == READ) {
        rw_mtx.unlock_shared(); //释放共享读锁
    } else if (mode == WRITE) {
        rw_mtx.unlock(); //释放独占写锁
    } else {
        std::cerr << "LockMode errno!" << std::endl;
        exit(EXIT_FAILURE); //这属于代码错误，直接退出程序
    }
}





