project(custom_accessor)

create_executable(${PROJECT_NAME})
target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>")