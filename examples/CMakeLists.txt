cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
add_compile_options(-std=c++1z)
project(KVANT_EXAMPLES VERSION 0.1.0 LANGUAGES CXX)

add_executable(
    empty_app
    empty_app.cpp
    ../src/backends/sdl2/imgui_impl_sdl.cpp
)
target_link_libraries(empty_app KvantEngine)
target_include_directories(empty_app PUBLIC
    ${COMMON_INCLUDE_DIRS}
)
