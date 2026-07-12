#pragma once
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "scotland2/shared/modloader.h"
#include <string>

namespace AppleMusicSearch {

static modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

Configuration& getConfig();
std::string getServerAddress();
void setServerAddress(const std::string& address);

}
