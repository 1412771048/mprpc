#include <iostream>
#include <typeinfo>

class Base {
public:
    virtual ~Base() {} // 虚析构函数使得 Base 成为多态类型
};

class Derived : public Base {
public:
    void sayHello() {
        std::cout << "Hello from Derived" << std::endl;
    }
};

int main() {
    Base* b = new Derived(); // Base 指针指向 Derived 对象

    // 使用 dynamic_cast 进行安全类型转换
    Derived* d = dynamic_cast<Derived*>(b);
    if (d) {
        d->sayHello(); // 成功转换
    } else {
        std::cout << "Failed to cast to Derived" << std::endl;
    }

    delete b;
    return 0;
}
