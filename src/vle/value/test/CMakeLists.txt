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

ADD_EXECUTABLE(test_values test1.cpp)
TARGET_LINK_LIBRARIES(test_values
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vledata
  vlevalue)

ADD_EXECUTABLE(test_values_serial test_serialization.cpp)
TARGET_LINK_LIBRARIES(test_values_serial
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ${Boost_SERIALIZATION_LIBRARY}
  vlegeometry
  vleutils
  vledata
  vlevalue)

ADD_TEST(valuetest_simple test_values)
ADD_TEST(valuetest_serialization test_values_serial)

