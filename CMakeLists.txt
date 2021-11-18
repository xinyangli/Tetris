cmake_minimum_required(VERSION 3.7)
project(Tetris)

set(CMAKE_C_STANDARD 11)
# add_definitions(-D_CRT_SECURE_NO_WARNINGS)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(src)