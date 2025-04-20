include(FetchContent)

# declare and populate stf library
FetchContent_Declare(
    stf
    GIT_REPOSITORY https://github.com/nathanstouffer/stf.git
    GIT_TAG a4278588d8c60be15a24d0d254183511f45f9c77
)
FetchContent_MakeAvailable(stf)

# declare and populate DiligentCore and DiligentTools
FetchContent_Declare(
    DiligentCore
    URL https://github.com/DiligentGraphics/DiligentCore/releases/download/v2.5.4/DiligentCore_v2.5.4.zip
    URL_HASH SHA256=c27638b2c0582fa44b26e36c2212b7ac5fb4bcac93a94b91e9e68ad23c5df04e
    SOURCE_DIR _deps/DiligentCore
)
FetchContent_Declare(
    DiligentTools
    GIT_REPOSITORY https://github.com/DiligentGraphics/DiligentTools.git
    GIT_TAG v2.5.4
    SOURCE_DIR _deps/DiligentTools
)
FetchContent_MakeAvailable(DiligentCore DiligentTools)