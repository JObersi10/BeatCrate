#pragma once
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include <string>

namespace AppleMusicSearch {

    Configuration& getConfig();

    // Server address e.g. "192.168.1.100:8080"
    std::string getServerAddress();
    void setServerAddress(const std::string& address);

}
