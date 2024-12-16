cmake_minimum_required(VERSION 3.23)

project(software-renderer)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-msse4.1)

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

enable_testing()

add_executable(vec_test test/vec_test.cpp ${IMPL} ${INCL})

target_include_directories(vec_test PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(vec_test PRIVATE ${PROJECT_SOURCE_DIR}/src/math)

target_link_libraries(vec_test Catch2::Catch2WithMain)

add_test(NAME vec_test COMMAND vec_test)

file(GLOB INCL ${PROJECT_SOURCE_DIR}/include/**)
file(GLOB IMPL ${PROJECT_SOURCE_DIR}/src/math/*)

add_executable(${PROJECT_NAME} main.cpp ${INCL} ${IMPL})
add_dependencies(${PROJECT_NAME} vec_test)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src/math)

target_link_libraries(${PROJECT_NAME} glfw gl3w)
