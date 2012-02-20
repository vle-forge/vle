ADD_EXECUTABLE(test_time time.cpp)

TARGET_LINK_LIBRARIES(test_time vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_TEST(devsttime test_time)
