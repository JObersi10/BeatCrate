include_guard()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/utils.cmake)

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/qpm.json PACKAGE_JSON)
string(JSON PACKAGE_INFO GET ${PACKAGE_JSON} info)
string(JSON PACKAGE_VERSION GET ${PACKAGE_INFO} version)
string(JSON EXTERN_DIR_NAME GET ${PACKAGE_JSON} dependenciesDir)
string(JSON SHARED_DIR_NAME GET ${PACKAGE_JSON} sharedDir)

set(EXTERN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${EXTERN_DIR_NAME})
set(SHARED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${SHARED_DIR_NAME})

cmake_language(DEFER DIRECTORY ${CMAKE_SOURCE_DIR} CALL _setup_qpm_project())
function(_setup_qpm_project)
    include(${CMAKE_CURRENT_SOURCE_DIR}/extern.cmake)
endfunction()
