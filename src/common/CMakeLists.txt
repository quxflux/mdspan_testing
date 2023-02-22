project(common)

add_library(${PROJECT_NAME} INTERFACE "inc/quxflux/common/wrap_mdspan.h" "inc/quxflux/common/concepts.h")
target_include_directories(${PROJECT_NAME} INTERFACE "inc")
target_link_libraries(${PROJECT_NAME} INTERFACE std::mdspan)
set_project_warnings(${PROJECT_NAME})
