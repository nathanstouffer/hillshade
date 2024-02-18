include(FetchContent)

# declare and populate stf library
FetchContent_Declare(
    stf
    GIT_REPOSITORY https://github.com/nathanstouffer/stf.git
    GIT_TAG 994fcb51455918825809f3c3bf41041b5db9ee86
)
FetchContent_MakeAvailable(stf)