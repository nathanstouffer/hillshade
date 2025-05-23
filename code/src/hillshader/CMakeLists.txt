set(HILLSHADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshader/camera/controllers/animators/animator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshader/camera/controllers/animators/orbit.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshader/camera/controllers/animators/zoom.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshader/camera/controllers/controller.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshader/camera/controllers/input.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshader/camera/constraints/collide.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshader/camera/physics/free_body.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshader/camera/physics/handler.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshader/camera/physics/orbit.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshader/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshader/application.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshader/terrain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshader/timer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hillshader/mesh.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/camera/config.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/camera/controllers/animators/animator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/camera/controllers/animators/orbit.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/camera/controllers/animators/zoom.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/camera/controllers/identity.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/camera/controllers/constant.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/camera/controllers/controller.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/camera/controllers/input.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/camera/constraints/collide.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/camera/physics/free_body.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/camera/physics/handler.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/camera/physics/orbit.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/application.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/terrain.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/timer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hillshader/mesh.hpp"
)

add_executable(hillshader WIN32 ${HILLSHADER_FILES})

# add directory structure to IDEs
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" FILES ${HILLSHADER_FILES})

file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "${CMAKE_CURRENT_BINARY_DIR}/shaders" SYMBOLIC)
file(CREATE_LINK "${CMAKE_SOURCE_DIR}/terrarium" "${CMAKE_CURRENT_BINARY_DIR}/terrarium" SYMBOLIC)

# TODO eventually we should split this out by platform
target_compile_definitions(hillshader
    PRIVATE
    PLATFORM_WIN32=1
)

# TODO figure out why we need to explicitly make this available here
FetchContent_MakeAvailable(DiligentCore DiligentTools)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Stb REQUIRED)

target_include_directories(hillshader
    PRIVATE
    "${diligentcore_SOURCE_DIR}"
    "${diligenttools_SOURCE_DIR}"
    "${Stb_INCLUDE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private"
)

# add small dependencies
target_link_libraries(hillshader
    PRIVATE
    stf
    nlohmann_json::nlohmann_json
)

# add diligent dependencies
target_link_libraries(hillshader
    PRIVATE
    Diligent-BuildSettings
    Diligent-Common
    Diligent-TextureLoader
    Diligent-Imgui
    # rendering dependencies
    Diligent-GraphicsEngineOpenGL-shared
)
copy_required_dlls(hillshader)

target_precompile_headers(hillshader PRIVATE <vector> <stf/stf.hpp>)