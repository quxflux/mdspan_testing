project(image_ops)

create_executable(${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/img/reporter-camera.ppm" ${PROJECT_BINARY_DIR}
)
