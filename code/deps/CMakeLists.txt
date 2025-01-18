include(FetchContent)

# declare and populate stf library
FetchContent_Declare(
    stf
    GIT_REPOSITORY https://github.com/nathanstouffer/stf.git
    GIT_TAG 4fbfb4a162cc57560758b7659e8c91bf896ca45b
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

find_package(gdal CONFIG REQUIRED)