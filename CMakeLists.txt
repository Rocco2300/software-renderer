cmake_minimum_required(VERSION 3.23)

project(software-renderer)

add_subdirectory(src/math)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} math gdi32)