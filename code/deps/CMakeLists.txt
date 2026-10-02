include(FetchContent)

# declare and populate stf library
FetchContent_Declare(
    stf
    GIT_REPOSITORY https://github.com/nathanstouffer/stf.git
    GIT_TAG 8f729b2cfa7af8a1c0e3b5b7c484ad050529f69f
)
FetchContent_MakeAvailable(stf)

# declare and populate DiligentCore and DiligentTools
FetchContent_Declare(
    DiligentCore
    GIT_REPOSITORY https://github.com/DiligentGraphics/DiligentCore.git
    GIT_TAG v2.5.6
    SOURCE_DIR _deps/DiligentCore
)
FetchContent_Declare(
    DiligentTools
    GIT_REPOSITORY https://github.com/DiligentGraphics/DiligentTools.git
    GIT_TAG v2.5.6
    SOURCE_DIR _deps/DiligentTools
)
FetchContent_MakeAvailable(DiligentCore DiligentTools)