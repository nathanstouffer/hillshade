set(HILLSHADE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshade/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshade/application.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshade/terrain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshade/mesh.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshade/application.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshade/terrain.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshade/mesh.hpp"
)

add_executable(hillshade WIN32 ${HILLSHADE_FILES})

# add directory structure to IDEs
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" FILES ${HILLSHADE_FILES})

file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "${CMAKE_CURRENT_BINARY_DIR}/shaders" SYMBOLIC)
file(CREATE_LINK "${CMAKE_SOURCE_DIR}/terrarium" "${CMAKE_CURRENT_BINARY_DIR}/terrarium" SYMBOLIC)

# TODO eventually we should split this out by platform
target_compile_definitions(hillshade
    PRIVATE
    PLATFORM_WIN32=1
    ENGINE_DLL=1
)

# TODO figure out why we need to explicitly make this available here
FetchContent_MakeAvailable(DiligentCore DiligentTools)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Stb REQUIRED)

target_include_directories(hillshade
    PRIVATE
    "${diligentcore_SOURCE_DIR}"
    "${diligenttools_SOURCE_DIR}"
    "${Stb_INCLUDE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private"
)

# add small dependencies
target_link_libraries(hillshade
    PRIVATE
    stf
    nlohmann_json::nlohmann_json
)

# add diligent dependencies
target_link_libraries(hillshade
    PRIVATE
    Diligent-BuildSettings
    Diligent-Common
    Diligent-TextureLoader
    Diligent-Imgui
    # rendering dependencies
    Diligent-GraphicsEngineOpenGL-shared
)
copy_required_dlls(hillshade)

target_precompile_headers(hillshade PRIVATE <vector> <stf/stf.hpp>)