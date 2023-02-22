project(mmap_accessor)

add_executable(${PROJECT_NAME} "src/main.cpp" "inc/quxflux/mmap_accessor/mmap_accessor.h" "src/write_mdspan.h")
target_link_libraries(${PROJECT_NAME} common mio::mio)
target_include_directories(${PROJECT_NAME} PRIVATE "inc" "src")

set_project_warnings(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20
                                                 CXX_STANDARD_REQUIRED ON
                                                 CXX_EXTENSIONS OFF)