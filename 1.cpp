#include <queue>
#include <mutex>
#include <condition_variable>


template<typename T>
class LockQueue {
public:
    LockQueue() = default;
    ~LockQueue() = default;
    void push(const T& data) {
        std::unique_lock lock(mtx_);
        que.push(data);
        cv_.notify_one();
    }
    const T pop() {
        std::unique_lock lock(mtx_);
        while (que.empty()) {
            cv_.wait(lock);
        }
        //出来后有数据且获取锁了
        const T data = que.front();
        que.pop();
        return data;
    }
private:
    std::queue que_;
    std::mutex mtx_;
    std::condition_variable cv_;
};


