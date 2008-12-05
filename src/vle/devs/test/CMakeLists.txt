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

ADD_LIBRARY(unittest SHARED models.cpp)
TARGET_LINK_LIBRARIES(unittest
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vledevs)

INSTALL(TARGETS unittest
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

ADD_EXECUTABLE(test_devsmodel test1.cpp)
TARGET_LINK_LIBRARIES(test_devsmodel
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vledevs
  vlevpz)

ADD_TEST(devstest test_devsmodel)
