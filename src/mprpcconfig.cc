#include "mprpcconfig.h"
#include <iostream>
#include <fstream>
#include <string>
#include "SimpleIni.h"

void MprpcConfig::LoadConfig() {
    CSimpleIniA ini;
    SI_Error rc = ini.LoadFile("../thirdparty/test.ini");
    if (rc < 0) {
        std::cerr << "Failed to load config file!" << std::endl;
        exit(EXIT_FAILURE);
    }
    config_map_["rpc_server_ip"] = ini.GetValue("rpc", "server_ip", "");
    config_map_["rpc_port"] = ini.GetValue("rpc", "port", "");
    config_map_["zookeeper_server_ip"] = ini.GetValue("zookeeper", "server_ip", "");
    config_map_["zookeeper_port"] = ini.GetValue("zookeeper", "port", "");
}


const std::string MprpcConfig::load(const std::string& key) {
    auto it = config_map_.find(key);
    if (it == config_map_.end()) {
        return "";
    }
    return it->second;
}

