set(HILLSHADE_SRC_FILES_PREFIX "cpp/hillshade")

set(SRC_FILES
    ${HILLSHADE_SRC_FILES_PREFIX}/main.cpp
)

add_executable(hillshade WIN32 ${SRC_FILES})

# TODO eventually we should split this out by platform
target_compile_definitions(hillshade
    PRIVATE
    PLATFORM_WIN32=1
)

target_include_directories(hillshade
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/private
    ${DiligentCore_SOURCE_DIR}
)

target_link_directories(hillshade
    PRIVATE
    ${DiligentCore_BINARY_DIR}
)

target_link_libraries(hillshade
    PRIVATE
    stf
    DiligentCore
)

# TODO figure out how to staticay link libraries? not sure if it's possible?
target_link_libraries(hillshade
    PRIVATE
    Diligent-BuildSettings
    Diligent-GraphicsEngineOpenGL-shared
)
copy_required_dlls(hillshade)

target_precompile_headers(hillshade PRIVATE <vector> <stf/stf.hpp>)