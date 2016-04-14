add_executable(test_values test1.cpp)
target_link_libraries(test_values vlelib ${Boost_LIBRARIES})
add_test(valuetest_simple test_values)
