enable_testing()

add_executable(vec_test vec_test.cpp ${IMPL} ${INCL})

target_include_directories(vec_test PUBLIC ${SOURCE_DIR}/include)

target_link_libraries(vec_test Catch2::Catch2WithMain)

add_executable(mat_test mat_test.cpp ${IMPL} ${INCL})

target_include_directories(mat_test PUBLIC ${SOURCE_DIR}/include)

target_link_libraries(mat_test Catch2::Catch2WithMain)

add_test(NAME vec_test COMMAND vec_test)
add_test(NAME mat_test COMMAND mat_test)
