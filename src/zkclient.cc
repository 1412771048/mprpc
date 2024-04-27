#include "zkclient.h"

//zkserver给zkclinet的通知
void watcher(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx) {
    if (type == ZOO_SESSION_EVENT && state == ZOO_CONNECTED_STATE) {//连接成功
            sem_post((sem_t*)zoo_get_context(zh));//信号量加1，主线程就解除阻塞了
    }
}
ZkClient::ZkClient(): zhandle_(nullptr) {

}

ZkClient::~ZkClient() {
    if (zhandle_ != nullptr) {
        zookeeper_close(zhandle_);//关闭句柄，释放资源
    }
}

void ZkClient::Start() {
    auto config_map_ptr = (std::unordered_map<std::string, std::string>*)RpcProvider::Lock("config_map", READ);
    std::string connect_str = (*config_map_ptr)["zookeeper_server_ip"] + ":" + (*config_map_ptr)["zookeeper_server_port"];
    RpcProvider::Unlock(READ);

    /* zookeeper_mt: 多线程版本,为什么用这个：zk的客户都安api提供了3个线程：
    1. api调用线程；2.网络io线程(poll，客户端程序，不需要高并发)；3.watcher回调线程    
    zookeeper_init(api调用线程)调用后会创建2个线程：网络io、watcher回调
    */
    //参数1：ip:host,参数2：watch回调，参数3：超时时间
    zhandle_ = zookeeper_init(connect_str.c_str(), watcher, 60000, nullptr, nullptr, 0);
    if (zhandle_ == nullptr) {
        std::cout << "zookeeper_init error" << std::endl;
        exit(EXIT_FAILURE);
    }
    //到这里表示创建句柄成功，不代表连接成功，因为这个init函数是异步的，所以需要用一个信号量来获取ZOO_CONNECTED_STATE
    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(zhandle_, &sem);
    sem_wait(&sem); //这里刚开始肯定阻塞
    std::cout << "zookeeper_init success" << std::endl;
}


void ZkClient::Create(const char* path, const char* data, int datalen, int state) {
    char path_buf[128] = {0};
    //先判断path位置是否存在节点
    if (zoo_exists(zhandle_, path, 0, nullptr) == ZNONODE) {
        //创建节点
        if(zoo_create(zhandle_, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state, path_buf, sizeof(path_buf)) == ZOK) {
            std::cout << "zookeeper_create success path:" << path << " content:" << data << std::endl;
        } else {
            std::cout << "zookeeper_create error" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

//对应get命令
std::string ZkClient::GetData(const char* path) {
    char buf[64] = {0};
    int buflen = sizeof(buf);
    if (zoo_get(zhandle_, path, 0, buf, &buflen, nullptr) != ZOK) {
        std::cout <<  "zookeeper_get error" << std::endl;
        return "";
    }
    return buf;
}