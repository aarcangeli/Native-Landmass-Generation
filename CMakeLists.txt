cmake_minimum_required(VERSION 3.12)
project(NativeLandmassGeneration)

set(CMAKE_CXX_STANDARD 11)
set(ROOT_DIRECTORY ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${ROOT_DIRECTORY}/cmake")
include(Variables)

add_subdirectory(src)
