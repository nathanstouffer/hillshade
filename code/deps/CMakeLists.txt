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
    URL https://github.com/DiligentGraphics/DiligentCore/archive/refs/tags/v2.5.4.zip
    URL_HASH SHA256=9d7dd6308ef4963aabd7a2ff909e42e735ec6f25cab69ef5e72b0eb345a3e4e1
    SOURCE_DIR _deps/DiligentCore
)
FetchContent_Declare(
    DiligentTools
    URL https://github.com/DiligentGraphics/DiligentTools/archive/refs/tags/v2.5.4.zip
    URL_HASH SHA256=95b874d1a322907c4a8845e07be17f1e5f96323c44947864f7e68e72e4c51bd1
    SOURCE_DIR _deps/DiligentTools
)
FetchContent_MakeAvailable(DiligentCore DiligentTools)