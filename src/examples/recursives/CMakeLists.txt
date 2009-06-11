INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/recursives)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

SET(SOURCES_CPP Simple.cpp TwoTimesModel.cpp)
  
ADD_LIBRARY(recursives SHARED ${SOURCES_CPP})

TARGET_LINK_LIBRARIES(recursives
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vlevpz
  vleoov
  vledevs
  vleextension
  vlemanager
  vletranslator)

INSTALL(TARGETS recursives
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

INSTALL(FILES recursive.vpz twotimesmodel.vpz DESTINATION
  "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES ${SOURCES_CPP} DESTINATION
  "${VLE_SHARE_DIRS}/examples/sources/recursives")

