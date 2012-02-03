ADD_EXECUTABLE(test_values test1.cpp)

TARGET_LINK_LIBRARIES(test_values vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_EXECUTABLE(test_values_serial test_serialization.cpp)

TARGET_LINK_LIBRARIES(test_values_serial vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_TEST(valuetest_simple test_values)

ADD_TEST(valuetest_serialization test_values_serial)

