cmake_minimum_required(VERSION 3.23)

project(software-renderer)

include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
)
FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
        GLFW
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(GLFW)

FetchContent_Declare(
        GL3W
        GIT_REPOSITORY https://github.com/skaslev/gl3w.git
)
FetchContent_MakeAvailable(GL3W)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} GLFW GL3W)

enable_testing()