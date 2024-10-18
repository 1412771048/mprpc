#include "rpcchannel.h"
#include "socket_pool.h"

namespace mprpc {
    //客户端的所有rpc请求都走这个，需要自己定义：我们在这里做数据序列化(填装proto)和网络发送
void MpRpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller, 
                    const google::protobuf::Message* request, google::protobuf::Message* response, google::protobuf::Closure* done)
{   
    //1. 序列化数据，组织要发送的字符流：header_size + rpc_header(service+method+args_size) + args_str
    auto [service_name, method_name, args_str] = std::make_tuple(method->service()->name(), method->name(), std::string(""));
    //把形参序列化,存入args_str
    if (!request->SerializeToString(&args_str)) {
        controller->SetFailed("形参serialize error!");
        return;
    }
    uint32_t args_size = args_str.size();
    //装填rpc_header的proto,序列化存入rpc_header_str
    mprpc::RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(args_size);
    std::string rpc_header_str;
    if (!rpc_header.SerializeToString(&rpc_header_str)) {
        controller->SetFailed("rpc_header serialize error!");
        return;
    }
    uint32_t header_size = rpc_header_str.size();

    std::string send_str;
    send_str.insert(0, std::string((char*)&header_size, 4)); //前4个字节放header_size的2进制比表示
    send_str += rpc_header_str + args_str;

    //2.查zk，获取rpc节点的ip+port，这里会查到nginx的
    //创建单例连接池，开10条连接
    auto& zk_pool = ZkClientPool::GetInstance(10);
    // 获取一个连接
    ZkClient* zk_cli = zk_pool.GetConnection();
    if (zk_cli == nullptr) {
        std::cerr << "Failed to get ZooKeeper connection!" << std::endl;
        return;
    }

    std::string method_path = std::string("/") + method->service()->name() + "/" + method->name();
    std::string ip_port = zk_cli->GetData(method_path.c_str());
    //归还
    zk_pool.ReleaseConnection(zk_cli);
    if (ip_port == "") {
        controller->SetFailed("method path not exist!");
        return;
    }
    int idx = ip_port.find(":");
    std::string ip = std::string(ip_port.begin(), ip_port.begin() + idx);
    uint16_t port = stoi(std::string(ip_port.begin() + idx + 1, ip_port.end()));


    //3、socket连接，send数据
    auto& socket_pool = SocketConnectionPool::GetInstance(10, ip, port); 
    int fd = socket_pool.GetConnection();

    // int fd = socket(AF_INET, SOCK_STREAM, 0);
    // if (fd == -1) {
    //     controller->SetFailed("socketfd create error!");
    //     return;
    // } 
    // sockaddr_in addr = {AF_INET, htons(port), inet_addr(ip.c_str())};
    // if (connect(fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
    //     controller->SetFailed("socket connect error");
    //     close(fd);
    //     return;
    // }

    if (send(fd, send_str.c_str(), send_str.size(), 0) == -1) {
        controller->SetFailed(strerror(errno));
        close(fd);
        return;
    }

    //4、recv响应，反序列化
    char recv_buf[1024] = {0};
    //设置recv超时时间: 3s
    timeval timeout{3, 0};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    int recv_size = recv(fd, recv_buf, sizeof(recv_buf), 0);
    // close(fd);

    // 放回队列
    socket_pool.ReleaseConnection(fd);

    if (recv_size < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) controller->SetFailed("socket recv timeout");
        else controller->SetFailed("socket recv error, recv_size = -1");
        return;
    } 
    else if (recv_size == 0) {
        controller->SetFailed("socket connect close! recv_size = 0");
        return;
    } 

    //不能用ParseFromString：recv_buf有\0后面就截断了，如recv_buf = {'a', '\0', 'b'}
    if (!response->ParseFromArray(recv_buf, recv_size)) {
        controller->SetFailed("response parse error!");
        return;
    } 
}
}