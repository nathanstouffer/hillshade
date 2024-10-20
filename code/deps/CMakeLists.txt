include(FetchContent)

# declare and populate stf library
FetchContent_Declare(
    stf
    GIT_REPOSITORY https://github.com/nathanstouffer/stf.git
    GIT_TAG 8ce14acd5e5db0be2195e3ad4d7eabfed8f57d6b
)
FetchContent_MakeAvailable(stf)

# declare and populate DiligentCore and DiligentTools
FetchContent_Declare(
    DiligentCore
    GIT_REPOSITORY https://github.com/DiligentGraphics/DiligentCore.git
    GIT_TAG v2.5.4
    SOURCE_DIR _deps/DiligentCore
)
FetchContent_Declare(
    DiligentTools
    GIT_REPOSITORY https://github.com/DiligentGraphics/DiligentTools.git
    GIT_TAG v2.5.4
    SOURCE_DIR _deps/DiligentTools
)
FetchContent_MakeAvailable(DiligentCore DiligentTools)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    message (STATUS "CMake thinks we are compiling for 64-bit")
else ()
    message (STATUS "CMake thinks we are compiling for something not 64-bit")
endif()
find_package(GeoTIFF CONFIG REQUIRED)