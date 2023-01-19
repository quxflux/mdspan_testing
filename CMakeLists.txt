cmake_minimum_required(VERSION 3.22)

project(mdspan_testing LANGUAGES CXX)

include(cmake/CompilerWarnings.cmake)

add_subdirectory(lib/mdspan)

add_library(base_project INTERFACE "include/ppm.h" "include/image.h" "include/wrap_mdspan.h" "include/layout_proxy.h" "include/layout_index_calculators.h")
include_directories(base_project PUBLIC "include")
target_link_libraries(base_project INTERFACE std::mdspan)

function(create_executable exe_name exe_main)
    add_executable(${exe_name} ${exe_main} )
    target_link_libraries(${exe_name} base_project)

    set_project_warnings(${exe_name})
    set_target_properties(${exe_name} PROPERTIES CXX_STANDARD 23
                                                 CXX_STANDARD_REQUIRED ON
                                                 CXX_EXTENSIONS OFF)
endfunction()

create_executable(example_image_ops "src/example_image_ops.cpp" )
add_custom_command(TARGET example_image_ops POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/img/reporter-camera.ppm ${PROJECT_BINARY_DIR}
)