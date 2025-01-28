add_subdirectory(src)

add_executable(${PROJECT_NAME} main.cpp ${SRC} ${INCL} ${IMPL})

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} glfw gl3w fast_obj src)

add_dependencies(${PROJECT_NAME} vec_test mat_test)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ctest.exe -C $<CONFIGURATION> --extra-verbose
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test
)
