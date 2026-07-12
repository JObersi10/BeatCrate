cmake_minimum_required(VERSION 3.22)

option(QUEST "Build for Quest" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_compile_options(-frtti -fexceptions -fvisibility=hidden -fPIE -fPIC)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/utils.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/qpm.cmake)

if(${CMAKE_BUILD_TYPE} MATCHES "^(Release|RelWithDebInfo|MinSizeRel)$")
    add_compile_options(-O3 -flto)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(QUEST)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/targets/quest.cmake)
endif()

project(apple-music-search VERSION ${PACKAGE_VERSION})

add_compile_definitions(MOD_ID="${CMAKE_PROJECT_NAME}")
add_compile_definitions(VERSION="${CMAKE_PROJECT_VERSION}")

set(COMPILE_ID ${CMAKE_PROJECT_NAME})

RECURSE_FILES(cpp_file_list ${SOURCE_DIR}/*.cpp)
RECURSE_FILES(c_file_list   ${SOURCE_DIR}/*.c)

add_library(${CMAKE_PROJECT_NAME} SHARED ${cpp_file_list} ${c_file_list})
target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_23)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${SHARED_DIR})

RECURSE_FILES(inline_hook_c   ${EXTERN_DIR}/includes/beatsaber-hook/shared/inline-hook/*.c)
RECURSE_FILES(inline_hook_cpp ${EXTERN_DIR}/includes/beatsaber-hook/shared/inline-hook/*.cpp)
target_sources(${COMPILE_ID} PRIVATE ${inline_hook_c} ${inline_hook_cpp})

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE log)

# Embed BSML asset files into assets.hpp
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/assets.cmake)
embed_assets(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/assets)
