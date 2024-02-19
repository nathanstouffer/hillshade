include(FetchContent)

# declare and populate stf library
FetchContent_Declare(
    stf
    GIT_REPOSITORY https://github.com/nathanstouffer/stf.git
    GIT_TAG 994fcb51455918825809f3c3bf41041b5db9ee86
)
FetchContent_MakeAvailable(stf)

# TODO (stouff) test out version 2.5.4
# declare and populate DiligentCore
FetchContent_Declare(
    DiligentCore
    GIT_REPOSITORY https://github.com/DiligentGraphics/DiligentCore.git
    GIT_TAG v2.5.4
    SOURCE_DIR _deps/DiligentCore
)
FetchContent_MakeAvailable(DiligentCore)

# declare and populate DiligentTools
FetchContent_Declare(
    DiligentTools
    GIT_REPOSITORY https://github.com/DiligentGraphics/DiligentTools.git
    GIT_TAG v2.5.4
    SOURCE_DIR _deps/DiligentTools
)
FetchContent_MakeAvailable(DiligentTools)