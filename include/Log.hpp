#pragma once
#include "beatsaber-hook/shared/utils/logging.hpp"

namespace AppleMusicSearch {
    static constexpr auto logger = Paper::ConstLoggerContext("AppleMusicSearch");
}

#define AMS_LOG(fmt, ...) AppleMusicSearch::logger.info(fmt, ##__VA_ARGS__)
#define AMS_ERROR(fmt, ...) AppleMusicSearch::logger.error(fmt, ##__VA_ARGS__)
#define AMS_WARN(fmt, ...) AppleMusicSearch::logger.warn(fmt, ##__VA_ARGS__)
#define AMS_DEBUG(fmt, ...) AppleMusicSearch::logger.debug(fmt, ##__VA_ARGS__)
