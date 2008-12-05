INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

ADD_EXECUTABLE(test_graph test1.cpp)
TARGET_LINK_LIBRARIES(test_graph
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vleoov
  vlevpz
  vledevs
  vleextension
  vlemanager
  vletranslator)

ADD_TEST(graphtest test_graph)
