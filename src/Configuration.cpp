#include "Configuration.hpp"
#include "Log.hpp"

namespace AppleMusicSearch {

Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

static std::string cfgGet(const char* key, const char* def = "") {
    auto& cfg = getConfig().config;
    if (cfg.HasMember(key) && cfg[key].IsString()) return cfg[key].GetString();
    return def;
}
static void cfgSet(const char* key, const std::string& val) {
    auto& cfg = getConfig().config;
    auto& alloc = cfg.GetAllocator();
    rapidjson::Value k(key, alloc), v(val.c_str(), alloc);
    if (!cfg.HasMember(key)) cfg.AddMember(k, v, alloc);
    else                     cfg[key].SetString(val.c_str(), alloc);
    getConfig().Write();
}

std::string getMut()                     { return cfgGet("mut"); }
void        setMut(const std::string& v) { cfgSet("mut", v); }
std::string getCachedJwt()               { return cfgGet("jwt"); }
void        setCachedJwt(const std::string& v) { cfgSet("jwt", v); }
std::string getStorefront()              { return cfgGet("storefront", "us"); }
void        setStorefront(const std::string& v){ cfgSet("storefront", v); }

}
