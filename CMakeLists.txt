cmake_minimum_required(VERSION 3.23)

project(software-renderer)

set(CMAKE_CXX_STANDARD 20)
set(TEST_DIR ${CMAKE_SOURCE_DIR}/test)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/software-renderer)

add_compile_options(-msse4.1)

file(GLOB SRC ${SOURCE_DIR}/src/*.cpp)
file(GLOB INCL ${SOURCE_DIR}/include/math/*)
file(GLOB IMPL ${SOURCE_DIR}/include/math/impl/*)

include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
)
FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        gl3w
        GIT_REPOSITORY https://github.com/skaslev/gl3w.git
)
FetchContent_MakeAvailable(gl3w)

add_subdirectory(${PROJECT_SOURCE_DIR}/test)
add_subdirectory(${PROJECT_SOURCE_DIR}/software-renderer)
