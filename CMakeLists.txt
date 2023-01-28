cmake_minimum_required(VERSION 3.22)

project(mdspan_testing LANGUAGES CXX)

include(cmake/CompilerWarnings.cmake)

add_subdirectory(lib/mdspan)

add_subdirectory(src/common)
add_subdirectory(src/image_ops)
