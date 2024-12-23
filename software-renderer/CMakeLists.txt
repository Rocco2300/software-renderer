add_subdirectory(src)

add_executable(${PROJECT_NAME} main.cpp ${SRC} ${INCL} ${IMPL})
add_dependencies(${PROJECT_NAME} vec_test mat_test)

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} glfw gl3w src)
