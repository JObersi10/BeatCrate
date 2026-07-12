#pragma once
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "scotland2/shared/modloader.h"
#include <string>

namespace AppleMusicSearch {

static modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

Configuration& getConfig();
std::string getMut();
void        setMut(const std::string& v);
std::string getCachedJwt();
void        setCachedJwt(const std::string& v);
std::string getStorefront();
void        setStorefront(const std::string& v);

}
