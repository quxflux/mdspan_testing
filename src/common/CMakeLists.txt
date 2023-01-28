project(common)

add_library(${PROJECT_NAME} INTERFACE "inc/quxflux/common/wrap_mdspan.h" "inc/quxflux/common/concepts.h")
target_include_directories(${PROJECT_NAME} INTERFACE "inc")
target_link_libraries(${PROJECT_NAME} INTERFACE std::mdspan)

function(create_executable exe_name)
    add_executable(${exe_name} "src/${exe_name}.cpp" )
    target_link_libraries(${exe_name} common)
    target_include_directories(${exe_name} PRIVATE "inc")

    set_project_warnings(${exe_name})
    set_target_properties(${exe_name} PROPERTIES CXX_STANDARD 23
                                                 CXX_STANDARD_REQUIRED ON
                                                 CXX_EXTENSIONS OFF)
endfunction()