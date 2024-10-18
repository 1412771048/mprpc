#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

class SocketConnectionPool {
public:
    SocketConnectionPool(const SocketConnectionPool&) = delete;
    SocketConnectionPool& operator=(const SocketConnectionPool&) = delete;
    SocketConnectionPool(SocketConnectionPool&&) = delete;
    SocketConnectionPool& operator=(SocketConnectionPool&&) = delete;
    ~SocketConnectionPool() {
        while (!pool_.empty()) {
            int sockfd = pool_.front();
            close(sockfd);
            pool_.pop();
        }
    }

    // 获取单例实例
    static SocketConnectionPool& GetInstance(int pool_size = 5, const std::string& server_ip = "127.0.0.1", uint16_t server_port = 8001) {
        static SocketConnectionPool instance(pool_size, server_ip, server_port);
        return instance;
    }

    // 获取连接
    int GetConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (pool_.empty()) {
            cond_.wait(lock);
        }
        int sockfd = pool_.front();
        pool_.pop();
        return sockfd;
    }

    // 归还连接
    void ReleaseConnection(int sockfd) {
        std::unique_lock<std::mutex> lock(mutex_);
        pool_.push(sockfd);
        cond_.notify_one();
    }

private:
    SocketConnectionPool(int pool_size, const std::string& server_ip, uint16_t server_port)
        : server_ip_(server_ip), server_port_(server_port) {
        for (int i = 0; i < pool_size; ) {
            int sockfd = create_socket_connection();
            if (sockfd != -1) {
                pool_.push(sockfd);
                ++i;
            } 
            else {
                std::cerr << "Failed to create socket connection, skipping..." << std::endl;
            }
        }
    }

    int create_socket_connection() {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
            return -1;
        }

        sockaddr_in server_addr{AF_INET, htons(server_port_), inet_addr(server_ip_.c_str())};

        if (connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            std::cerr << "Failed to connect to server: " << strerror(errno) << std::endl;
            close(sockfd);
            return -1;
        }
        return sockfd;
    }

    std::string server_ip_;
    uint16_t server_port_;
    std::queue<int> pool_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

// // 使用示例
// int main() {
//     // 获取连接池实例并初始化（自动通过构造函数完成初始化）
//     auto& socket_pool = SocketConnectionPool::GetInstance(10, "127.0.0.1", 8080);  // 初始化池大小为 10 的连接池

//     // 获取一个连接
//     int fd = socket_pool.GetConnection();
//     if (fd == -1) {
//         std::cerr << "Failed to get socket connection!" << stdendl;
//         return -1;
//     }

//     // 使用 fd 进行操作...
//     const char *msg = "Hello, server!";
//     ssize_t result = send(fd, msg, strlen(msg), 0);
//     if (result == -1) {
//         std::cerr << "Send failed: " << strerror(errno) << std::endl;
//     } else {
//         char buffer[1024] = {0};
//         recv(fd, buffer, 1024, 0);
//         std::cout << "Response from server: " << buffer << std::endl;
//     }

//     // 使用完后将连接归还到连接池
//     socket_pool.ReleaseConnection(fd);

//     return 0;
// }
