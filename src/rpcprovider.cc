#include "rpcprovider.h"

std::shared_mutex RpcProvider::rw_mtx; 
bool RpcProvider::config_ok = false;
std::unordered_map<std::string, std::string> RpcProvider::config_map;
std::unordered_map<std::string, google::protobuf::Service*> RpcProvider::service_map;


void* RpcProvider::Lock(const std::string& data, const LockMode& mode) {
    if (mode == READ) {
        rw_mtx.lock_shared(); //获取共享读锁
    } else if (mode == WRITE) {
        rw_mtx.lock(); //获取独占写锁
    } else {
        std::cerr << "LockMode errno!" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (data == "config_map") {
        return &config_map;
    } else if (data == "service_map") {
        return &service_map;
    } else {
        std::cerr << "data is not exist!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return nullptr;
}   

void RpcProvider::Unlock(const LockMode& mode) {
    if (mode == READ) {
        rw_mtx.unlock_shared(); //释放共享读锁
    } else if (mode == WRITE) {
        rw_mtx.unlock(); //释放独占写锁
    } else {
        std::cerr << "LockMode errno!" << std::endl;
        exit(EXIT_FAILURE); //这属于代码错误，直接退出程序
    }
}

void RpcProvider::Init(int argc, char** argv) {
    if (config_ok) {
        return;
    }

	int opt = 0;
	std::string conf = "";
	while ((opt = getopt(argc, argv, "i:")) != -1) {
		switch (opt) {
			case 'i':
                conf = optarg;
				break;
		}
	}
    if (conf.empty()) {
        std::cerr << "error! please input format: command -i <config_path>" << std::endl;	
        exit(EXIT_FAILURE);
    }


    CSimpleIniA ini;
    SI_Error rc = ini.LoadFile(conf.c_str());
    if (rc < 0) {
        std::cerr << conf << "加载失败，请检查输入的文件绝对路径！" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string rpc_server_ip = ini.GetValue("rpc", "server_ip", "");
    std::string rpc_server_port = ini.GetValue("rpc", "server_port", "");
    std::string zookeeper_server_ip = ini.GetValue("zookeeper", "server_ip", "");
    std::string zookeeper_server_port = ini.GetValue("zookeeper", "server_port", "");
    if (rpc_server_ip.empty() || rpc_server_port.empty() || zookeeper_server_ip.empty() || zookeeper_server_port.empty()) {
        std::cerr << conf << "内容无效，请检查！" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    auto config_map_ptr = (std::unordered_map<std::string, std::string>*)Lock("config_map", WRITE);
    config_map_ptr->insert({"rpc_server_ip", rpc_server_ip});
    config_map_ptr->insert({"rpc_server_port", rpc_server_port});
    config_map_ptr->insert({"zookeeper_server_ip", zookeeper_server_ip});
    config_map_ptr->insert({"zookeeper_server_port", zookeeper_server_port});
    Unlock(WRITE);

    config_ok = true;
}

void RpcProvider::NotifyService(google::protobuf::Service *service) {
   //服务名-服务描述
    auto service_map_ptr = (std::unordered_map<std::string, google::protobuf::Service*>*)Lock("service_map", WRITE);
    service_map_ptr->insert({service->GetDescriptor()->name(), service});
    RpcProvider::Unlock(WRITE);
}
  
//启动网络服务，即muduo库编程
void RpcProvider::Run() {
    //先拿ip, port
    auto config_map_ptr = (std::unordered_map<std::string, std::string>*)Lock("config_map", READ);
    std::string ip = (*config_map_ptr)["rpc_server_ip"];
    uint16_t port = stoi((*config_map_ptr)["rpc_server_port"]);
    RpcProvider::Unlock(READ);

    MuduoStart(ip, port, "RpcProvider", 4); //启动muduo网络服务
}

void RpcProvider::MuduoStart(const std::string& ip, const uint16_t port, const std::string& server_name, uint8_t thread_num) {
    //muduo库编程，基本死的
    muduo::net::TcpServer server(&event_loop, muduo::net::InetAddress(ip, port), server_name);//第3个参数就是个标识，服务器名
    // 绑定连接回调和消息读写回调，分离网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    //设置线程数
    server.setThreadNum(thread_num);//一个io线程，三个工作线程
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
        LOG_ERROR("反序列化头部失败！");
        return;
    }
    std::string service_name = rpc_header.service_name();
    std::string method_name = rpc_header.method_name();
    int args_size = rpc_header.args_size(); 

    //查表判断有无服务和方法
    auto service_map_ptr = (std::unordered_map<std::string, google::protobuf::Service*>*)Lock("service_map", READ);
    //判断服务是否存在
    auto it = service_map_ptr->find(service_name);
    if (it == service_map_ptr->end()) {
        LOG_ERROR("service_name not exist!");
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
        LOG_ERROR("%s not exist!", method_name);
        return;
    }
    //获取方法
    const google::protobuf::MethodDescriptor* method = desc->method(method_index);

    RpcProvider::Unlock(READ); 

    //方法服务都有了，开始封装请求
    auto request = service->GetRequestPrototype(method).New();
    //request把剩下的参数的字符流给反序列化
    if (!request->ParseFromString(recv_str.substr(4 + header_size, args_size))) {
        LOG_ERROR("request 反序列化失败！");
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

