cmake_minimum_required(VERSION 2.6)

project (Altair-emu)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})


add_subdirectory(lib)
add_subdirectory(app)
