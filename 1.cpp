#include <unordered_map>
using namespace std;

class LRUCache {
public:
    //删除最久未使用的，查找O(1):哈希表，维护最久未使用:链表
    LRUCache(int capacity): capacity(capacity), head(new Node()), tail(new Node()) {
        head->next = tail;
        tail->pre = head;
    }

    ~LRUCache() {
        while (head) {
            Node* t = head;
            head = head->next;
            delete t;
        }
    }

    int get(int key) {
        //查哈希表
        auto it = map.find(key);
        if (it != map.end()) {
            moveHead(it->second);
            return it->second->v;
        }
        return -1;
    }
    
    void put(int key, int value) {
        auto it = map.find(key);
        if (it != map.end()) {
            it->second->v = value;
            moveHead(it->second);
        }
        else {
            Node* t = new Node(key, value, head, head->next);
            map.insert({key, t});
            head->next = t;
            t->next->pre = t;
            if (map.size() > capacity) {
                //删除尾部元素
                Node* p = tail->pre;
                map.erase(p->k);

                p->pre->next = tail;
                tail->pre = p->pre;
                delete p;
            }
        }
    }
private:
    struct Node {
        int k, v;
        Node* pre, *next;
        Node(int k = 0, int v = 0, Node* pre= nullptr, Node* next = nullptr): k(k), v(v), pre(pre), next(next) {}
    };
    unordered_map<int, Node*> map;
    Node* head, *tail; //2个虚拟头节点
    int capacity;

    void moveHead(Node* p) {
        p->pre->next = p->next;
        p->next->pre = p->pre;
        p->next = head->next;
        p->next->pre = p;
        p->pre = head;
        head->next = p;
    }
};

int main() {
    LRUCache t(10);
}