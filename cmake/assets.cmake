# Generate assets.hpp from files in the assets/ directory.
# Text files (.bsml, .xml, etc.) are embedded as C++11 raw string literals.
# Binary files (.png, etc.) are embedded as hex byte arrays.

function(embed_assets TARGET ASSETS_DIR)
    file(GLOB ASSET_FILES "${ASSETS_DIR}/*")

    set(OUT_HPP "${CMAKE_CURRENT_BINARY_DIR}/assets.hpp")

    file(WRITE "${OUT_HPP}" "#pragma once\n#include <string_view>\nnamespace IncludedAssets {\n")

    foreach(F IN LISTS ASSET_FILES)
        get_filename_component(FNAME "${F}" NAME)
        get_filename_component(FEXT  "${F}" EXT)
        string(REPLACE "." "_" VARNAME "${FNAME}")

        if(FEXT STREQUAL ".bsml" OR FEXT STREQUAL ".xml" OR FEXT STREQUAL ".json" OR FEXT STREQUAL ".txt")
            # Text file — embed as raw string literal (delimiter BCASSET is unlikely to appear in XML)
            file(READ "${F}" CONTENT)
            file(APPEND "${OUT_HPP}" "inline constexpr std::string_view ${VARNAME} = R\"BCASSET(${CONTENT})BCASSET\";\n")
        else()
            # Binary file — emit as uint8_t array + string_view over it
            file(READ "${F}" CONTENT HEX)
            string(REGEX REPLACE "([0-9a-fA-F][0-9a-fA-F])" "0x\\1," BYTES "${CONTENT}")
            file(APPEND "${OUT_HPP}" "inline constexpr uint8_t _${VARNAME}_data[] = {${BYTES}};\n")
            file(APPEND "${OUT_HPP}" "inline constexpr std::string_view ${VARNAME}(reinterpret_cast<const char*>(_${VARNAME}_data), sizeof(_${VARNAME}_data));\n")
        endif()
    endforeach()

    file(APPEND "${OUT_HPP}" "}\n")

    target_include_directories(${TARGET} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()
