INCLUDE_DIRECTORIES(
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

ADD_EXECUTABLE(test_experimentalframes test1.cpp)
TARGET_LINK_LIBRARIES(test_experimentalframes
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNITTESTFRAMEWORK}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vledevs
  vlevpz
  vlemanager)

ADD_TEST(managertest test_experimentalframes)
