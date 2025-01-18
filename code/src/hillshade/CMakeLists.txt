set(HILLSHADE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshade/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshade/application.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshade/terrain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshade/application.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshade/terrain.h"
)

add_executable(hillshade WIN32 ${HILLSHADE_FILES})

message(STATUS "Copying resources")
file(COPY "${CMAKE_SOURCE_DIR}/tiffs" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")

# add directory structure to IDEs
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" FILES ${HILLSHADE_FILES})

# TODO eventually we should split this out by platform
target_compile_definitions(hillshade
    PRIVATE
    PLATFORM_WIN32=1
    ENGINE_DLL=1
)

# TODO figure out why we need to explicitly make this available here
FetchContent_MakeAvailable(DiligentCore DiligentTools)
find_package(gdal CONFIG REQUIRED)

target_include_directories(hillshade
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private"
    "${diligentcore_SOURCE_DIR}"
    "${diligenttools_SOURCE_DIR}"
    "${DILIGENT_DEAR_IMGUI_PATH}"
    "${gdal_INCLUDE_DIRS}"
)

target_link_directories(hillshade
    PRIVATE
    "${diligentcore_BINARY_DIR}"
    "${diligenttools_BINARY_DIR}"
)

target_link_libraries(hillshade
    PRIVATE
    stf
    DiligentCore
    DiligentTools
    GDAL::GDAL
)

# TODO figure out how to staticly link libraries? not sure if it's possible?
target_link_libraries(hillshade
    PRIVATE
    Diligent-BuildSettings
    Diligent-GraphicsEngineOpenGL-shared
)
copy_required_dlls(hillshade)

target_precompile_headers(hillshade PRIVATE <vector> <stf/stf.hpp>)