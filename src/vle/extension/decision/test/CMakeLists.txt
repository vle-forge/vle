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

ADD_EXECUTABLE(kb kb.cpp)
ADD_EXECUTABLE(activity activity.cpp)
ADD_EXECUTABLE(allenrelation allenrelation.cpp)
ADD_EXECUTABLE(parser parser.cpp)
ADD_EXECUTABLE(ss ss.cpp)
ADD_LIBRARY(modelsimple SHARED modelsimple.cpp)

TARGET_LINK_LIBRARIES(kb
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vleoov
  vlevpz
  vledevs
  vleextension
  vlemanager)

TARGET_LINK_LIBRARIES(activity
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vleoov
  vlevpz
  vledevs
  vleextension
  vlemanager)

TARGET_LINK_LIBRARIES(allenrelation
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vleoov
  vlevpz
  vledevs
  vleextension
  vlemanager)

TARGET_LINK_LIBRARIES(parser
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vleoov
  vlevpz
  vledevs
  vleextension
  vlemanager)

TARGET_LINK_LIBRARIES(ss
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vleoov
  vlevpz
  vledevs
  vleextension
  vlemanager)

TARGET_LINK_LIBRARIES(modelsimple
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${VLE_LIBRARIES}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vleoov
  vlevpz
  vledevs
  vleextension
  vlemanager)

ADD_TEST(extension_decision_kb kb)
ADD_TEST(extension_decision_activity activity)
ADD_TEST(extension_decision_allen allenrelation)
ADD_TEST(extension_decision_parser parser)
