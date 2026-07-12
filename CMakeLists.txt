cmake_minimum_required(VERSION 3.22)
project(apple-music-search)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(DEFINED_BS_QUEST "Build for Quest" ON)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(extern/libs.cmake OPTIONAL RESULT_VARIABLE libs_result)
if(NOT libs_result)
    message(STATUS "No extern/libs.cmake found — run 'qpm restore' first")
endif()

file(GLOB_RECURSE SOURCE_FILES
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/**/*.cpp"
)

add_library(apple-music-search SHARED ${SOURCE_FILES})

target_include_directories(apple-music-search PRIVATE
    ${INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/build
)

target_compile_options(apple-music-search PRIVATE
    -frtti
    -fexceptions
    -fPIC
    -fvisibility=hidden
    -O3
    -flto
)

target_compile_definitions(apple-music-search PRIVATE
    MOD_ID="${PROJECT_NAME}"
    VERSION="1.0.0"
)

target_link_libraries(apple-music-search
    log
    ssl
    crypto
)

include(cmake/assets.cmake OPTIONAL)
