set(HILLSHADE_SRC_FILES_PREFIX "cpp/hillshade")

set(SRC_FILES
    ${HILLSHADE_SRC_FILES_PREFIX}/main.cpp
)

add_executable(hillshade ${SRC_FILES})

target_include_directories(hillshade PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/private)

target_link_libraries(hillshade PRIVATE stf)

target_precompile_headers(hillshade PRIVATE <vector> <stf/types.hpp>)