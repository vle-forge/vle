ADD_EXECUTABLE(test_algo test1.cpp)

TARGET_LINK_LIBRARIES(test_algo vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_EXECUTABLE(test_template test_template.cpp)

TARGET_LINK_LIBRARIES(test_template vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_EXECUTABLE(test_parser test_parser.cpp)

TARGET_LINK_LIBRARIES(test_parser vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_TEST(utilstest_algo test_algo)
ADD_TEST(utilstest_template test_template)
ADD_TEST(utilstest_parser test_parser)
