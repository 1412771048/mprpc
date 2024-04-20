#include "rpcprovider.h"

void RpcProvider::LoadConfig() {
    CSimpleIniA ini;
    SI_Error rc = ini.LoadFile("../../config/test.ini");
    if (rc < 0) {
        std::cerr << "Failed to open config file!" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string rpc_server_ip = ini.GetValue("rpc", "server_ip", "");
    std::string rpc_server_port = ini.GetValue("rpc", "server_port", "");
    std::string zookeeper_server_ip = ini.GetValue("zookeeper", "server_ip", "");
    std::string zookeeper_server_port = ini.GetValue("zookeeper", "server_port", "");
    if (rpc_server_ip.empty() || rpc_server_port.empty() || zookeeper_server_ip.empty() || zookeeper_server_port.empty()) {
        std::cerr << "config file format errno!" << std::endl;
        exit(EXIT_FAILURE);
    }

    auto config_map_ptr = (std::unordered_map<std::string, std::string>*)DataBank::Lock("config_map", DataBank::WRITE);

    config_map_ptr->insert({"rpc_server_ip", rpc_server_ip});
    config_map_ptr->insert({"rpc_server_port", rpc_server_port});
    config_map_ptr->insert({"zookeeper_server_ip", zookeeper_server_ip});
    config_map_ptr->insert({"zookeeper_server_port", zookeeper_server_port});

    DataBank::Unlock(DataBank::WRITE);
}

void RpcProvider::NotifyService(google::protobuf::Service *service) {
    //获取服务的具体描述：里面有服务名，方法数，每个方法名，都通过这个拿
    auto pserverdesc = service->GetDescriptor(); 
    std::string service_name = pserverdesc->name(); //获取服务名,即类名
    int method_count = pserverdesc->method_count(); //获取方法数量，因为一个服务对象可以有多个方法

    ServiceInfo service_info; 
    service_info.service = service; 
    for (int i = 0; i < method_count; i++) {
        auto pmethoddesc = pserverdesc->method(i); //获取一个方法描述
        std::string method_name = pmethoddesc->name(); //获取方法名，即函数名
        service_info.methodMap[method_name] = pmethoddesc;
    }   
    m_serviceMap.insert({service_name, service_info}); //插入服务信息到服务map中


    //服务名-服务描述
    auto service_map_ptr = (std::unordered_map<std::string, google::protobuf::Service*>*)DataBank::Lock("service_map", DataBank::WRITE);
    service_map_ptr->insert({service->GetDescriptor()->name(), service});
    DataBank::Unlock(DataBank::WRITE);
}
  
//启动网络服务，即muduo库编程
void RpcProvider::Run() {
    //先拿ip, port
    auto config_map_ptr = (std::unordered_map<std::string, std::string>*)DataBank::Lock("config_map", DataBank::READ);
    std::string ip = (*config_map_ptr)["rpc_server_ip"];
    uint16_t port = stoi((*config_map_ptr)["rpc_server_port"]);
    DataBank::Unlock(DataBank::READ);

    //muduo库编程，基本死的
    muduo::net::TcpServer server(&event_loop, muduo::net::InetAddress(ip, port), "RpcProvider");//第3个参数就是个标识，服务器名
    // 绑定连接回调和消息读写回调，分离网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    //设置线程数
    server.setThreadNum(4);//一个io线程，三个工作线程
    //启动网络服务,后续有连接请求时，会调用connectioncallback，有消息读写请求时，会调用messagecallback
    server.start();
    event_loop.loop();
}

//对于新连接我们要干嘛
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn) {
    if (!conn->connected()) {//断开连接了
        conn->shutdown(); //关闭文件描述符，对应socket的close
    }
}

//对于读写事件我们要干嘛，不同的场景不一样，这里就是反序列化，执行函数
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time) {
    //拿到请求的字节流：xxxxxxxxxx
    std::string recv_str = buf->retrieveAllAsString();
    //我们要反序列化它：根据proto定义的规则来干
    //先从字符流中读前4个字节
    uint32_t header_size = 0;
    recv_str.copy((char*)&header_size, 4, 0); //从0位置拷贝4个字节放到整数里，就会显示成10进制
    //拿到rpchead的字符流
    std::string rpcheader_str = recv_str.substr(4, header_size);
    //反序列化
    mprpc::RpcHeader rpc_header;
    if (!rpc_header.ParseFromString(rpcheader_str)) {
        std::cerr << "反序列化头部失败！" << std::endl;
        return;
    }
    std::string service_name = rpc_header.service_name();
    std::string method_name = rpc_header.method_name();
    int args_size = rpc_header.args_size(); 

    //查表判断有无服务和方法
    auto service_map_ptr = (std::unordered_map<std::string, google::protobuf::Service*>*)DataBank::Lock("service_map", DataBank::READ);
    //判断服务是否存在
    auto it = service_map_ptr->find(service_name);
    if (it == service_map_ptr->end()) {
        std::cerr << service_name << "not exist!" << std::endl;
        return;
    }
    google::protobuf::Service* service = it->second; //服务
    //服务存在，再判断方法是否存在
    auto desc = service->GetDescriptor();
    uint8_t method_index = -1;
    for (int i = 0; i < desc->method_count(); ++i) {    
        if (desc->method(i)->name() == method_name) {
            method_index = i; 
            break;
        }
    }
    if (method_index == -1) {
        std::cerr << method_name << "not exist!" << std::endl;
        return;
    }
    //获取方法
    const google::protobuf::MethodDescriptor* method = desc->method(method_index);

    DataBank::Unlock(DataBank::READ); 

    //方法服务都有了，开始封装请求
    auto request = service->GetRequestPrototype(method).New();
    //request把剩下的参数的字符流给反序列化
    if (!request->ParseFromString(recv_str.substr(4 + header_size, args_size))) {
        std::cerr << "request 反序列化失败！" << std::endl;
        return;
    }

    auto response = service->GetResponsePrototype(method).New();
    
    //这个done给下面的CallMethod用的，有很多重载，这里选择：
    /*  template <typename Class, typename Arg1, typename Arg2>
        inline Closure* NewCallback(Class* object, void (Class::*method)(Arg1, Arg2), Arg1 arg1, Arg2 arg2) {
            return new internal::MethodClosure2<Class, Arg1, Arg2>(object, method, true, arg1, arg2);
        } */
    //他是模板肯定要指定类型，然后再传参啊！
    auto done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
                (this, &RpcProvider::SendRpcResponse, conn, response);

    //所有的rpc方法都走这个回调,传done是为了给rpc方法的done->run提供方法，
    service->CallMethod(method, nullptr, request, response, done);
}   

//此函数专门用于给done使用
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response) {
    //把序列化后的响应发送出去
    conn->send(response->SerializeAsString());
    //发送完断开，模拟http短连接，给更多的客户端提供请求
    conn->shutdown(); 
}

