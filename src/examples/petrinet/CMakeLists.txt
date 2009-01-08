INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/petrinet)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

ADD_LIBRARY(petrinet SHARED Simple.cpp)

TARGET_LINK_LIBRARIES(petrinet
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
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

INSTALL(TARGETS petrinet
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

INSTALL(FILES petrinet-and.vpz petrinet-mult.vpz petrinet-or.vpz
  petrinet-nand1.vpz petrinet-nand2.vpz petrinet-and-timed.vpz
  petrinet-or-priority.vpz petrinet-simple.vpz petrinet-meteo.vpz
  petrinet-inout.vpz petrinet-conflict.vpz DESTINATION
  "${VLE_SHARE_DIRS}/examples")
