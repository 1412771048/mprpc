// #include <signal.h>
// #include "rpcprovider.h"

// #include "user.pb.h"

// //客户端(rpc调用方)编程示例：
// int main(int argc, char** argv) {
//     // //1.初始化框架
//     auto& config = mprpc::MpRpcConfig::GetInstance();
//     config.Init(argc, argv);

//     mprpc::MprpcController controller; //记录rpc调用中的状态和错误信息
//     mprpc::MpRpcChannel channel;
//     fixbug::RpcService_Stub stub(&channel);

//     while (1) {
//         std::cout << "1: 查询, 2: 增删改" << std::endl;
//         std::cout << "Input choice:" << std::endl;
//         std::string str;
//         std::getline(std::cin, str);
//         try {
//             uint8_t choice = std::stoi(str);
//             if (choice == 1) {
//                 fixbug::QueryRequest request;
//                 fixbug::QueryResponse response;

//                 std::cout << "Input sql:" << std::endl;
//                 std::string sql;
//                 std::getline(std::cin, sql);
//                 if (sql[sql.size() - 1] == ';') sql.pop_back();
//                 request.set_sql(sql);

//                 stub.Query(&controller, &request, &response, nullptr);
//                 if (controller.Failed()) std::cerr << controller.ErrorText() << std::endl;
//                 else if (response.success() == -1) std::cout << "query failed" << std::endl;
//                 else {
//                     std::cout << "查到" << response.success() << "条记录" << std::endl;
//                     auto res = response.mutable_msg();
//                     for (int i = 0; i < res->lists_size(); i++) {
//                         auto vec = res->lists(i);
//                         for (int j = 0; j < vec.items_size(); j++) {
//                             std::cout << vec.items(j) << " ";
//                         }
//                         std::cout << std::endl;
//                     }
//                 }
//             } 
//             else if (choice == 2) {
//                 fixbug::UpdateRequest request;
//                 fixbug::UpdateResponse response;

//                 std::cout << "Input sql:" << std::endl;
//                 std::string sql;
//                 std::getline(std::cin, sql);
//                 if (sql[sql.size() - 1] == ';') sql.pop_back();
//                 request.set_sql(sql);

//                 stub.Update(&controller, &request, &response, nullptr);
//                 if (controller.Failed()) std::cerr << controller.ErrorText() << std::endl;
//                 std::cout << response.msg() << std::endl;
//                 if (response.success()) {
//                     //update sucess
//                 } else {
//                     //update sucess   
//                 }            
//             } 
//             else std::cerr << "输入错误" << std::endl;

//             controller.Reset();
//         } 
//         catch(...) {
//             std::cerr << "输入错误" << std::endl;
//         }
//         std::cout << std::endl;
//     }
// } 


#include <signal.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include "rpcprovider.h"
#include "user.pb.h"

// 全局变量，用于统计请求数量
std::atomic<int> total_requests(0);
std::atomic<int> successful_requests(0);

std::atomic<int> num(0); //用于update的数字

// 模拟并发请求的线程函数
void SendRequests(int thread_id, int num_requests, bool is_query) {
    mprpc::MprpcController controller; // 每个线程一个controller
    mprpc::MpRpcChannel channel;
    fixbug::RpcService_Stub stub(&channel);
    for (int i = 0; i < num_requests; ++i) {
        if (is_query) {
            fixbug::QueryRequest request;
            fixbug::QueryResponse response;
            // 模拟查询 SQL 语句
            request.set_sql("select * from person;");
            stub.Query(&controller, &request, &response, nullptr);
            if (controller.Failed()) {
                std::cerr << "Thread " << thread_id << " Query failed: " << controller.ErrorText() << std::endl;
            } 
            else {
                successful_requests++;
            }
        } 
        else {
            fixbug::UpdateRequest request;
            fixbug::UpdateResponse response;
            char sql[100] = {0};
            sprintf(sql, "update person set age=%d where name='gyl'", ++num);
            // 模拟更新 SQL 语句
            request.set_sql(sql);
            stub.Update(&controller, &request, &response, nullptr);
            if (controller.Failed()) {
                std::cerr << "Thread " << thread_id << " Update failed: " << controller.ErrorText() << std::endl;
            } 
            else {
                successful_requests++;
            }
        }
        total_requests++;
        controller.Reset(); // 重置 controller，以便下一次使用
    }
}

int main(int argc, char** argv) {
    // 初始化框架
    auto& config = mprpc::MpRpcConfig::GetInstance();
    config.Init(argc, argv);

    int num_threads = 10;           // 并发线程数
    int requests_per_thread = 1000; // 每个线程发起的请求数
    bool is_query = true;          // true 表示执行查询操作，false 表示执行更新操作

    // 记录测试开始时间
    auto start_time = std::chrono::high_resolution_clock::now();
    // 创建并启动线程
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(SendRequests, i, requests_per_thread, is_query);
        is_query = !is_query;
    }
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    // 记录测试结束时间
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start_time;
    // 计算 QPS
    double qps = successful_requests / elapsed.count();
    std::cout << "Total Requests Sent: " << total_requests.load() << std::endl;
    std::cout << "Successful Requests: " << successful_requests.load() << std::endl;
    std::cout << "Elapsed Time: " << elapsed.count() << " seconds" << std::endl;
    std::cout << "QPS: " << qps << std::endl;
}
