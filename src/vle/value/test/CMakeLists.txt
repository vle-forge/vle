add_executable(test_values test1.cpp)

target_link_libraries(test_values vlelib ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(valuetest_simple test_values)
