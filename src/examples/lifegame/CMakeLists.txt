INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/lifegame)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

SET(SOURCES_CPP Executive.cpp Executive.hpp Cell.cpp Cell.hpp)

ADD_LIBRARY(lifegame SHARED ${SOURCES_CPP})

TARGET_LINK_LIBRARIES(lifegame
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

INSTALL(TARGETS lifegame
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

INSTALL(FILES lifegame_txt.vpz lifegame.vpz DESTINATION
  "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES ${SOURCES_CPP} DESTINATION
  "${VLE_SHARE_DIRS}/examples/sources/lifegame")
