add_library(src window.cpp texture.cpp mesh.cpp)

target_include_directories(src PUBLIC ${SOURCE_DIR}/include)

target_link_libraries(src glfw gl3w fast_obj)
