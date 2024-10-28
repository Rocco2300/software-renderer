cmake_minimum_required(VERSION 3.23)

project(software-renderer)

add_compile_options(-msse4.1)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src/math)

target_link_libraries(${PROJECT_NAME} gdi32)

include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
)
FetchContent_MakeAvailable(Catch2)

enable_testing()