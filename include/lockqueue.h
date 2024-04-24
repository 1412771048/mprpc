#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>


//异步写日志的队列,在stl的queue基础上封装线程安全，所有用上了模板
template<typename T> //模板的声明和定义要写在一个文件里
class LockQueue {
public:
    //多个线程push
    void push(const T& data) {
        std::unique_lock<std::mutex> lock(mtx_);
        queue_.push(data);
        condition_.notify_one(); //通知一个线程，出括号释放锁
    }

    //一个线程pop，为什么用while:防止多线程pop空队列，导致死锁
    T pop() {
        //pop取队列：若队列空，释放锁，阻塞等待唤醒
        std::unique_lock<std::mutex> lock(mtx_);
        while(queue_.empty()) {
            condition_.wait(lock);
        }
        //收到信号，被唤醒，重新获取锁，出循环
        T data = queue_.front();
        queue_.pop();
        return data;
    }
private:
    std::queue<T> queue_;
    std::mutex mtx_;
    std::condition_variable condition_;
};