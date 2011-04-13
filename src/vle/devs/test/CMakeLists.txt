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

ADD_LIBRARY(DevsDeleteConnection SHARED DevsDeleteConnection.cpp)
TARGET_LINK_LIBRARIES(DevsDeleteConnection
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vledevs)

INSTALL(TARGETS DevsDeleteConnection
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

ADD_LIBRARY(DevsTransform SHARED DevsTransform.cpp)
TARGET_LINK_LIBRARIES(DevsTransform
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vledevs)

INSTALL(TARGETS DevsTransform
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

ADD_EXECUTABLE(test_time time.cpp)
TARGET_LINK_LIBRARIES(test_time
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vledevs
  vlevpz)

ADD_TEST(devsttime test_time)
