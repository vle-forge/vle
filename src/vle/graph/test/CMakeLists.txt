ADD_EXECUTABLE(test_graph test1.cpp)

TARGET_LINK_LIBRARIES(test_graph vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_TEST(graphtest test_graph)
