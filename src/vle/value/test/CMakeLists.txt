add_executable(test_values test1.cpp)
target_link_libraries(test_values vlelib ${CMAKE_THREAD_LIBS_INIT})
add_test(valuetest_simple test_values)
