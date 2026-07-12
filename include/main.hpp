#pragma once
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "scotland2/shared/modloader.h"

#define MOD_EXPORT __attribute__((visibility("default")))
#define MOD_EXTERN_FUNC extern "C" MOD_EXPORT
