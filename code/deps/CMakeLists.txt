include(FetchContent)

# declare and populate stf library
FetchContent_Declare(
    stf
    GIT_REPOSITORY https://github.com/nathanstouffer/stf.git
    GIT_TAG 35e0c3793b3d8f3ac1fed31120196bd7269591d0
)
FetchContent_MakeAvailable(stf)