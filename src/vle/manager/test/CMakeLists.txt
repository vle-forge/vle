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

ADD_EXECUTABLE(test_experimentalframes test1.cpp)
TARGET_LINK_LIBRARIES(test_experimentalframes
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vledevs
  vlevpz
  vlemanager)

ADD_EXECUTABLE(test_combination_storage test2.cpp)
TARGET_LINK_LIBRARIES(test_combination_storage
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vledevs
  vlevpz
  vlemanager)

ADD_EXECUTABLE(test_common_seed test3.cpp)
TARGET_LINK_LIBRARIES(test_common_seed
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vledevs
  vlevpz
  vlemanager)

ADD_TEST(manager_test_exp test_experimentalframes)
ADD_TEST(manager_combination_storage test_combination_storage)
ADD_TEST(manager_common_seed test_common_seed)
