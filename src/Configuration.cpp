#include "Configuration.hpp"
#include "Log.hpp"

static Configuration _config;
static bool _loaded = false;

namespace AppleMusicSearch {

Configuration& getConfig() {
    if (!_loaded) {
        _config.Load();
        _loaded = true;
    }
    return _config;
}

std::string getServerAddress() {
    auto& cfg = getConfig().config;
    if (cfg.HasMember("serverAddress") && cfg["serverAddress"].IsString()) {
        return cfg["serverAddress"].GetString();
    }
    return "192.168.1.100:8080";
}

void setServerAddress(const std::string& address) {
    auto& cfg = getConfig().config;
    if (!cfg.HasMember("serverAddress")) {
        cfg.AddMember("serverAddress",
                      rapidjson::Value(address.c_str(), cfg.GetAllocator()),
                      cfg.GetAllocator());
    } else {
        cfg["serverAddress"].SetString(address.c_str(), cfg.GetAllocator());
    }
    getConfig().Write();
    AMS_LOG("Server address saved: {}", address);
}

}
