#pragma once 
#include <semaphore> //信号量， c++20
#include <string>
#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <memory>
#include <condition_variable>
#include <functional>

#include "zookeeper/zookeeper.h"

#include "rpcconfig.h"

namespace mprpc {
//封装zk客户端
class ZkClient {
public:
    ZkClient();
    ~ZkClient();
    //启动客户端，连接server
    int Start(const std::string& zk_ip_port); 
    //在path位置create节点，state表示节点类型,默认是永久节点
    int Create(const char* path, const char* data, int datalen, int state = 0); 
    //对应get命令
    std::string GetData(const char* path);
private:
    //zk客户端句柄
    zhandle_t *zhandle_;
};

//zk连接池
class ZkClientPool {
public:
    // 获取单例实例
    static ZkClientPool& GetInstance(uint8_t pool_size) {
        static ZkClientPool instance(pool_size);
        return instance;
    }

    // 获取连接
    ZkClient* GetConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (pool_.empty()) {
            cond_.wait(lock);
        }
        ZkClient* zk_client = pool_.front();
        pool_.pop();
        return zk_client;
    }

    // 释放连接
    void ReleaseConnection(ZkClient* zk_client) {
        std::unique_lock<std::mutex> lock(mutex_);
        pool_.push(zk_client);
        cond_.notify_one();
    }
    
    ~ZkClientPool() {
        while (!pool_.empty()) {
            delete pool_.front();
            pool_.pop();
        }
    }

private:
    ZkClientPool(uint8_t pool_size) {
        const std::string zk_ip_port = mprpc::MpRpcConfig::GetInstance().QuerryConfig("zk_ip_port");

        for (size_t i = 0; i < pool_size; ++i) {
            ZkClient* zk_client = new ZkClient();
            if (zk_client->Start(zk_ip_port) == 0) {
                pool_.push(zk_client);
            } else {
                delete zk_client;
                std::cerr << "Failed to connect to ZooKeeper, skipping..." << std::endl;
            }
        }
    }

    ZkClientPool(const ZkClientPool&) = delete;
    ZkClientPool& operator=(const ZkClientPool&) = delete;
    ZkClientPool(ZkClientPool&&) = delete;
    ZkClientPool& operator=(ZkClientPool&&) = delete;
    
    std::queue<ZkClient*> pool_;
    std::mutex mutex_;
    std::condition_variable cond_;
};


} //namespace mprpc