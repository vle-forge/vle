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

ADD_EXECUTABLE(test_vpz_values test1.cpp)
TARGET_LINK_LIBRARIES(test_vpz_values
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vleutils
  vlevalue
  vlegraph
  vledevs
  vlevpz)

ADD_EXECUTABLE(test_vpz_project test2.cpp)
TARGET_LINK_LIBRARIES(test_vpz_project
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vleutils
  vlevalue
  vlegraph
  vledevs
  vlevpz)

ADD_EXECUTABLE(test_vpz_translator test3.cpp)
TARGET_LINK_LIBRARIES(test_vpz_translator
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vleutils
  vlevalue
  vlegraph
  vledevs
  vlevpz)

ADD_EXECUTABLE(test_vpz_io test4.cpp)
TARGET_LINK_LIBRARIES(test_vpz_io
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vleutils
  vlevalue
  vlegraph
  vledevs
  vlevpz)

ADD_EXECUTABLE(test_vpz_oov test5.cpp)
TARGET_LINK_LIBRARIES(test_vpz_oov
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vleutils
  vlevalue
  vlegraph
  vledevs
  vlevpz)

ADD_EXECUTABLE(test_vpz_classes test6.cpp)
TARGET_LINK_LIBRARIES(test_vpz_classes
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vleutils
  vlevalue
  vlegraph
  vledevs
  vlevpz)

ADD_EXECUTABLE(test_vpz_graph test7.cpp)
TARGET_LINK_LIBRARIES(test_vpz_graph
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vleutils
  vlevalue
  vlegraph
  vledevs
  vlevpz)

ADD_TEST(vpztest_values test_vpz_values)
ADD_TEST(vpztest_project test_vpz_project)
ADD_TEST(vpztest_translator test_vpz_translator)
ADD_TEST(vpztest_io test_vpz_io)
ADD_TEST(vpztest_oov test_vpz_oov)
ADD_TEST(vpztest_classes test_vpz_classes)
ADD_TEST(vpztest_structures test_vpz_graph)
