project(image_ops)

add_executable(${PROJECT_NAME} "src/main.cpp" )
target_link_libraries(${PROJECT_NAME} common)
target_include_directories(${PROJECT_NAME} PRIVATE "inc")

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20
                                                 CXX_STANDARD_REQUIRED ON
                                                 CXX_EXTENSIONS OFF)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/img/reporter-camera.ppm" ${PROJECT_BINARY_DIR}
)
