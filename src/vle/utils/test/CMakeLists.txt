INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

ADD_EXECUTABLE(test_algo test1.cpp)
TARGET_LINK_LIBRARIES(test_algo
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ${Boost_DATE_TIME_LIBRARY}
  vleutils
  vlevalue
  vlegraph
  vleoov
  vlevpz
  vledevs
  vlemanager
  vletranslator)

ADD_EXECUTABLE(test_template test_template.cpp)
TARGET_LINK_LIBRARIES(test_template
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ${Boost_DATE_TIME_LIBRARY}
  vleutils
  vlevalue
  vlegraph
  vleoov
  vlevpz
  vledevs
  vlemanager
  vletranslator)

ADD_EXECUTABLE(test_parser test_parser.cpp)
TARGET_LINK_LIBRARIES(test_parser
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ${Boost_DATE_TIME_LIBRARY}
  vleutils
  vlevalue
  vlegraph
  vleoov
  vlevpz
  vledevs
  vlemanager
  vletranslator)

ADD_TEST(utilstest_algo test_algo)
ADD_TEST(utilstest_template test_template)
ADD_TEST(utilstest_parser test_parser)
