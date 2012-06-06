add_executable(test_values test1.cpp)

target_link_libraries(test_values vlelib ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(test_values_serial test_serialization.cpp)

target_link_libraries(test_values_serial vlelib ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(valuetest_simple test_values)

add_test(valuetest_serialization test_values_serial)

