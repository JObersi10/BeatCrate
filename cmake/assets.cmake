# Generate assets.hpp from .bsml files in the assets/ directory.
# Uses a Python one-liner so no cmake regex is needed.

function(embed_assets TARGET ASSETS_DIR)
    file(GLOB BSML_FILES "${ASSETS_DIR}/*.bsml")

    set(OUT_HPP "${CMAKE_CURRENT_BINARY_DIR}/assets.hpp")

    # Build python script as a cmake string to avoid escaping issues
    set(PY_SCRIPT [=[
import sys, os, pathlib
out = pathlib.Path(sys.argv[1])
assets_dir = pathlib.Path(sys.argv[2])
lines = ["#pragma once\n", "#include <string_view>\n", "namespace IncludedAssets {\n"]
for f in sorted(assets_dir.glob("*.bsml")):
    varname = f.name.replace(".", "_")
    content = f.read_text(encoding="utf-8")
    # Use a raw-string delimiter unlikely to appear in XML
    lines.append(f"inline constexpr std::string_view {varname} = R\"BCASSET({content})BCASSET\";\n")
lines.append("}\n")
out.write_text("".join(lines), encoding="utf-8")
print(f"Generated {out} with {len(lines)-3} assets")
]=])

    set(PY_SCRIPT_PATH "${CMAKE_CURRENT_BINARY_DIR}/gen_assets.py")
    file(WRITE "${PY_SCRIPT_PATH}" "${PY_SCRIPT}")

    execute_process(
        COMMAND python3 "${PY_SCRIPT_PATH}" "${OUT_HPP}" "${ASSETS_DIR}"
        RESULT_VARIABLE PY_RESULT
        OUTPUT_VARIABLE PY_OUTPUT
        ERROR_VARIABLE  PY_ERROR
    )

    if(NOT PY_RESULT EQUAL 0)
        message(FATAL_ERROR "gen_assets.py failed:\n${PY_ERROR}")
    endif()
    message(STATUS "${PY_OUTPUT}")

    target_include_directories(${TARGET} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()
