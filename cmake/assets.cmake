# Generate assets.hpp from files in the assets/ directory.
# Each file becomes a std::string_view IncludedAssets::<name> where <name> is
# the filename with dots replaced by underscores (e.g. Foo_bsml).

function(embed_assets TARGET ASSETS_DIR)
    file(GLOB ASSET_FILES "${ASSETS_DIR}/*")

    set(OUT_HPP "${CMAKE_CURRENT_BINARY_DIR}/assets.hpp")

    # Build a cmake script that regenerates the header at build time.
    set(GEN_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/gen_assets.cmake")

    # Write the generator script (runs at build time via add_custom_command).
    file(WRITE "${GEN_SCRIPT}" "
set(OUT \"${OUT_HPP}\")
file(WRITE \"\${OUT}\" \"#pragma once\\n#include <string_view>\\nnamespace IncludedAssets {\\n\")
foreach(F IN LISTS ASSET_FILES)
    get_filename_component(FNAME \"\${F}\" NAME)
    string(REPLACE \".\" \"_\" VARNAME \"\${FNAME}\")
    file(READ \"\${F}\" CONTENT HEX)
    string(REGEX REPLACE \"([0-9a-f][0-9a-f])\" \"\\\\\\\\x\\\\1\" ESCAPED \"\${CONTENT}\")
    file(APPEND \"\${OUT}\" \"inline constexpr std::string_view \${VARNAME} = \\\"\" )
    file(APPEND \"\${OUT}\" \"\${ESCAPED}\")
    file(APPEND \"\${OUT}\" \"\\\";\\n\")
endforeach()
file(APPEND \"\${OUT}\" \"}\\n\")
")

    # Actually write it directly at configure time too so the header exists
    # before the compiler is invoked.
    set(ASSET_FILES_LIST "${ASSET_FILES}")
    file(WRITE "${OUT_HPP}" "#pragma once\n#include <string_view>\nnamespace IncludedAssets {\n")
    foreach(F IN LISTS ASSET_FILES)
        get_filename_component(FNAME "${F}" NAME)
        string(REPLACE "." "_" VARNAME "${FNAME}")
        file(READ "${F}" CONTENT HEX)
        string(REGEX REPLACE "([0-9a-f][0-9a-f])" "\\x\\1" ESCAPED "${CONTENT}")
        file(APPEND "${OUT_HPP}" "inline constexpr std::string_view ${VARNAME} = \"${ESCAPED}\";\n")
    endforeach()
    file(APPEND "${OUT_HPP}" "}\n")

    # Re-run at build time if any asset file changes.
    add_custom_command(
        OUTPUT "${OUT_HPP}"
        COMMAND ${CMAKE_COMMAND}
                -D "ASSET_FILES=${ASSET_FILES}"
                -P "${GEN_SCRIPT}"
        DEPENDS ${ASSET_FILES}
        COMMENT "Regenerating assets.hpp"
    )
    add_custom_target(generate_assets DEPENDS "${OUT_HPP}")
    add_dependencies(${TARGET} generate_assets)

    target_include_directories(${TARGET} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()
