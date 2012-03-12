ADD_EXECUTABLE(test_vpz_values test1.cpp)

TARGET_LINK_LIBRARIES(test_vpz_values vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_EXECUTABLE(test_vpz_project test2.cpp)

TARGET_LINK_LIBRARIES(test_vpz_project vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_EXECUTABLE(test_vpz_translator test3.cpp)

TARGET_LINK_LIBRARIES(test_vpz_translator vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_EXECUTABLE(test_vpz_io test4.cpp)

TARGET_LINK_LIBRARIES(test_vpz_io vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_EXECUTABLE(test_vpz_oov test5.cpp)

TARGET_LINK_LIBRARIES(test_vpz_oov vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_EXECUTABLE(test_vpz_classes test6.cpp)

TARGET_LINK_LIBRARIES(test_vpz_classes vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_EXECUTABLE(test_vpz_structures test7.cpp)

TARGET_LINK_LIBRARIES(test_vpz_structures vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_EXECUTABLE(test_vpz_graph test8.cpp)

TARGET_LINK_LIBRARIES(test_vpz_graph vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_TEST(vpztest_values test_vpz_values)
ADD_TEST(vpztest_project test_vpz_project)
ADD_TEST(vpztest_translator test_vpz_translator)
ADD_TEST(vpztest_io test_vpz_io)
ADD_TEST(vpztest_oov test_vpz_oov)
ADD_TEST(vpztest_classes test_vpz_classes)
ADD_TEST(vpztest_structures test_vpz_structures)
ADD_TEST(vpztest_graph test_vpz_graph)
