#include <unordered_map>
#include <string>
#include <shared_mutex>


//这个类专门存放公共的数据供大家读写
class DataBank {
public:
    static std::shared_mutex config_map_mutex; //读写锁
    static std::unordered_map<std::string, std::string> config_map;
private:


    static std::unordered_map<std::string, std::shared_mutex*> data_mutex_map;
};


