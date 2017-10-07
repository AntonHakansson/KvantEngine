cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
add_compile_options(-std=c++1z)
project(KvantEngine VERSION 0.1.0 LANGUAGES CXX)

find_package(PkgConfig REQUIRED)

option(PLATFORM_IS_SDL "PLATFORM_IS_SDL" ON)
# option(PLATFORM_IS_GLFW "PLATFORM_IS_GLFW" ON)
option(GRAPHICS_IS_OPENGL "GRAPHICS_IS_OPENGL" ON)

option(BUILD_EXAMPLES_TOO "BUILD_EXAMPLES_TOO" ON)

# set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
pkg_search_module(Eigen3 REQUIRED eigen3)
set(BACKEND_INCLUDE_DIRS ${BACKEND_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR})

if (PLATFORM_IS_SDL)
    add_definitions(-DPLATFORM_IS_SDL)
    find_package(SDL2 REQUIRED)
    set(SOURCE_FILES ${SOURCE_FILES} src/backends/sdl2/sdl2.cpp)
    set(BACKEND_LIBS ${BACKEND_LIBS} SDL2)
endif()

if (GRAPHICS_IS_OPENGL)
    find_package(OpenGL REQUIRED)
    set(BACKEND_LIBS ${BACKEND_LIBS} GL)
endif()

set(SOURCE_FILES
     ${SOURCE_FILES}
        src/utils/Logger.hpp
        src/core/Loader.cpp
        src/imgui/imgui.cpp
        src/imgui/imgui_draw.cpp
        src/imgui/imgui_kvant.cpp
            src/utils/lodepng.cpp)

add_library(
    KvantEngine STATIC
        ${SOURCE_FILES}
)

add_definitions( -DDEBUG_LOGS -DINFO_LOGS -DWARNING_LOGS -DERROR_LOGS )

target_include_directories(KvantEngine
    PUBLIC
    ${BACKEND_INCLUDE_DIRS}
    src
    src/backends/blueprints
    src/core
    src/utils
    src/imgui
)

set (COMMON_LIBS
        ${BACKEND_LIBS})

set (COMMON_INCLUDE_DIRS
    ${BACKEND_INCLUDE_DIRS}
    src
    src/backends/blueprints
    src/core
    src/utils
    src/imgui
)

set (KvantEngine_RESOURCES_DIR
  res/
)

target_link_libraries(KvantEngine INTERFACE ${COMMON_LIBS})

if (BUILD_EXAMPLES_TOO)
  add_subdirectory(examples)
endif()