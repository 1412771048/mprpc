#include <iostream>
#include <cstdlib>  // 包含 system 函数

int postData(std::string data) {
    std::string cmd =  std::string("curl -d \"") + data + "\" " + "http://localhost:8080";
    std::cout << cmd << std::endl;
    return system(cmd.c_str());
}

int main() {
    std::string data = "12312424";
    if (postData(data) != 0) {
        std::cout << "failed" << std::endl;
    }
    else {
        std::cout << "sucess" << std::endl;
    }
}
