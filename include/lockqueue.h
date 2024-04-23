#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>


//异步写日志的队列,在stl的queue基础上封装线程安全，所有用上了模板
template<typename T>
class LockQueue {
public:
    void push(const T& data); 
    T& pop();
private:
    std::queue<T> queue_;
    std::mutex mtx_;
    std::condition_variable condition_;
};