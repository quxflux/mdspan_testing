cmake_minimum_required(VERSION 3.22)

project(mdspan_testing LANGUAGES CXX)

include(cmake/CompilerWarnings.cmake)

add_subdirectory(lib/mdspan)

add_executable(mdspan_testing "src/main.cpp" "src/ppm.h" "src/image.h" "src/wrap_mdspan.h" "src/layout_proxy.h" "src/layout_index_calculators.h")
include_directories(mdspan_testing src)
target_link_libraries(mdspan_testing std::mdspan)

set_project_warnings(mdspan_testing)
set_target_properties(mdspan_testing PROPERTIES CXX_STANDARD 23
                                                CXX_STANDARD_REQUIRED ON
                                                CXX_EXTENSIONS OFF)
