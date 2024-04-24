#include "loger.h"

Loger::Loger() {
    //构造函数启动一个写线程，把日志写入磁盘
    std::thread write_log([&](){
        std::string file_name = "";
        auto time = GetTime();
        while (1) {
            file_name = "../log/" + time.first + ".txt";
            FILE* fp = fopen(file_name.c_str(), "a+"); //以追加和读取的权限打开文件
            if (!fp) {
                std::cerr << "open file:" << file_name << " error!" << std::endl;
                exit(EXIT_FAILURE);
            }
            std::string msg = time.second + lock_queue.pop(); //调用pop无数据就会阻塞
            fputs(msg.c_str(), fp);
            fclose(fp);  
        }
    });
    write_log.detach();
}

std::pair<std::string, std::string> Loger::GetTime() {
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm *localTime = std::localtime(&currentTime);
    std::stringstream s1, s2;
    s1 << std::put_time(localTime, "%Y-%m-%d"); // 年-月-日 时 格式
    s2 << std::put_time(localTime, "%H:%M:%S "); //时分秒
    return {s1.str(), s2.str()};
}
Loger& Loger::GetInstance() {
    static Loger loger;
    return loger;
}
void Loger::SetLogLevel(LogLevel level) {
    log_level = level;
}
void Loger::Log(const std::string& msg) {
    lock_queue.push(msg);
}