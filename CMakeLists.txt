cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
add_compile_options(-std=c++1z)
project(KvantEngine VERSION 0.1.0 LANGUAGES CXX)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors")

find_package(PkgConfig REQUIRED)

option(PLATFORM_IS_SDL "PLATFORM_IS_SDL" ON)
# option(PLATFORM_IS_GLFW "PLATFORM_IS_GLFW" ON)
option(GRAPHICS_IS_OPENGL "GRAPHICS_IS_OPENGL" ON)
option(LOADER_IS_ASSIMP "LOADER_IS_ASSIMP" ON)

option(BUILD_EXAMPLES_TOO "BUILD_EXAMPLES_TOO" ON)

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

if (LOADER_IS_ASSIMP)
    include_directories(libs/assimp-3.1.1/include/)
    set (COMMON_LIBS ${COMMON_LIBS} assimp)
    set(BUILD_ASSIMP_TOOLS  ON)
    set(ASSIMP_BUILD_STATIC_LIB ON)
    add_subdirectory(libs/assimp-3.1.1)
endif()

file(GLOB_RECURSE SOURCE_FILES src/*)

add_library(
    KvantEngine STATIC
        ${SOURCE_FILES}
)

add_definitions( -DDEBUG_LOGS -DINFO_LOGS -DWARNING_LOGS -DERROR_LOGS )

target_include_directories(KvantEngine
    PUBLIC
    ${BACKEND_INCLUDE_DIRS}
    src
)

set (COMMON_LIBS
        ${COMMON_LIBS}
        ${BACKEND_LIBS}
)

set (COMMON_INCLUDE_DIRS
    ${BACKEND_INCLUDE_DIRS}
    src
)

set (KVANTENGINE_RESOURCES_PATH "${CMAKE_CURRENT_LIST_DIR}/res/")
add_definitions(-DKVANTENGINE_RESOURCES_PATH=\("${KVANTENGINE_RESOURCES_PATH}"\))
message("Resources set to ${KVANTENGINE_RESOURCES_PATH}")

target_link_libraries(KvantEngine INTERFACE ${COMMON_LIBS} ${data})

if (BUILD_EXAMPLES_TOO)
  add_subdirectory(examples)
endif()
