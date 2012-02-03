ADD_EXECUTABLE(test_manager test1.cpp)

TARGET_LINK_LIBRARIES(test_manager vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_TEST(manager_test test_manager)
