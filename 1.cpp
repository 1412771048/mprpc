#include <memory>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <semaphore>

template<typename T>
class LockFreeQueue {
private:
    struct Node {
        std::shared_ptr<T> data;
        Node* next;

        Node() : next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    LockFreeQueue() : head(new Node), tail(head.load()) {}
    ~LockFreeQueue() {
        while (Node* const old_head = head.load()) {
            head.store(old_head->next);
            delete old_head;
        }
    }

    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&) = delete;

    std::shared_ptr<T> pop() {
        Node* old_head = head.load();
        while (old_head != tail.load()) {
            if (head.compare_exchange_weak(old_head, old_head->next)) {
                std::shared_ptr<T> const res(old_head->data);
                delete old_head;
                return res;
            }
        }
        return std::shared_ptr<T>();
    }

    void push(T new_value) {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
        Node* p = new Node;
        Node* const old_tail = tail.load();

        old_tail->data.swap(new_data);
        old_tail->next = p;
        tail.store(p);
    }
};



LockFreeQueue<int> queue; //无锁队列
std::atomic<bool> done(false); //原子操作吗

void producer(int start) {
    for (int i = 0; i < 100; ++i) {
        queue.push(start + i);
    }
}

void consumer() {
    while (!done.load()) {
        std::shared_ptr<int> item = queue.pop();
        if (item) {
            std::cout << "Consumed: " << *item << std::endl;
        } else {
            // 没有元素可以消费，让出CPU时间
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

int main() {
    std::thread producers[2];
    std::thread consumers[2];

    // 启动生产者线程
    for (int i = 0; i < 2; ++i) {
        producers[i] = std::thread(producer, i * 100);
    }

    // 启动消费者线程
    for (int i = 0; i < 2; ++i) {
        consumers[i] = std::thread(consumer);
    }

    // 等待生产者完成
    for (auto& p : producers) {
        p.join();
    }

    done = true;

    // 等待消费者完成
    for (auto& c : consumers) {
        c.join();
    }

    return 0;
}