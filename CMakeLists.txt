cmake_minimum_required(VERSION 3.23)

project(software-renderer)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} gdi32)