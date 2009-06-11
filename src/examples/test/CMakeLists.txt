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

ADD_EXECUTABLE(test_gens testgens.cpp)
TARGET_LINK_LIBRARIES(test_gens
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vledevs
  vlevpz
  vlemanager)

ADD_EXECUTABLE(test_petrinet testpetrinet.cpp)
TARGET_LINK_LIBRARIES(test_petrinet
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vledevs
  vlevpz
  vlemanager)

ADD_EXECUTABLE(test_dess testdess.cpp)
TARGET_LINK_LIBRARIES(test_dess
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vledevs
  vlevpz
  vlemanager)

ADD_EXECUTABLE(test_equation testequation.cpp)
TARGET_LINK_LIBRARIES(test_equation
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vledevs
  vlevpz
  vlemanager)

ADD_EXECUTABLE(test_qss testqss.cpp)
TARGET_LINK_LIBRARIES(test_qss
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vledevs
  vlevpz
  vlemanager)

ADD_TEST(exemples_generator test_gens)
ADD_TEST(examples_petrinet test_petrinet)
ADD_TEST(examples_dess test_dess)
ADD_TEST(examples_equation test_equation)
ADD_TEST(examples_qss test_qss)
