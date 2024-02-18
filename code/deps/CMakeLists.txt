include(FetchContent)

# declare and populate stf library
FetchContent_Declare(
    stf
    GIT_REPOSITORY https://github.com/nathanstouffer/stf.git
    GIT_TAG 6b0e03c5ab6b41f830d924c89acbbe342ae2b7a1
)
FetchContent_MakeAvailable(stf)