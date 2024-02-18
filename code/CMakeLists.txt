cmake_minimum_required(VERSION 3.24)

project(hillshade VERSION 0.0.0 DESCRIPTION "An exploration in hillshading topographic maps" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

add_subdirectory(deps)
add_subdirectory(src)