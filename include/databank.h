#include <unordered_map>
#include <string>
#include <shared_mutex>
#include "google/protobuf/service.h"


//这个类专门存放公共的数据供大家读写
class DataBank {
public:
    typedef enum LockMode {
        READ,
        WRITE
    } LockMode;
    static void* Lock(const std::string& data, const LockMode& mode);
    static void Unlock(const LockMode& mode);

private:
    static std::shared_mutex rw_mtx; //一把读写锁控制这里的所有数据，这里明显读多写少,就用读写锁吧

    static std::unordered_map<std::string, std::string> config_map;
    static std::unordered_map<std::string, google::protobuf::Service*> service_map;
};


