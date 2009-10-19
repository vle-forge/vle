INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/fsa)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

SET(SOURCES1_CPP Generator.cpp)

ADD_LIBRARY(generator2 SHARED ${SOURCES1_CPP})

TARGET_LINK_LIBRARIES(generator2
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vlevpz
  vleoov
  vledevs
  vleextension
  vlemanager
  vletranslator)

INSTALL(TARGETS generator2
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

SET(SOURCES2_CPP Moore.cpp)

ADD_LIBRARY(moore SHARED ${SOURCES2_CPP})

TARGET_LINK_LIBRARIES(moore
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vlevpz
  vleoov
  vledevs
  vleextension
  vlemanager
  vletranslator)

INSTALL(TARGETS moore
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

SET(SOURCES3_CPP Mealy.cpp)

ADD_LIBRARY(mealy SHARED ${SOURCES3_CPP})

TARGET_LINK_LIBRARIES(mealy
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vlevpz
  vleoov
  vledevs
  vleextension
  vlemanager
  vletranslator)

INSTALL(TARGETS mealy
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

SET(SOURCES4_CPP Statechart.cpp)

ADD_LIBRARY(statechart SHARED ${SOURCES4_CPP})

TARGET_LINK_LIBRARIES(statechart
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vlevpz
  vleoov
  vledevs
  vleextension
  vlemanager
  vletranslator)

INSTALL(TARGETS statechart
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

SET(SOURCES5_CPP FDDevs.cpp)

ADD_LIBRARY(fddevs SHARED ${SOURCES5_CPP})

TARGET_LINK_LIBRARIES(fddevs
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vlevpz
  vleoov
  vledevs
  vleextension
  vlemanager
  vletranslator)

INSTALL(TARGETS fddevs
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

SET(SOURCES6_CPP Stage.cpp Stage.hpp Meteo.cpp)

ADD_LIBRARY(stage SHARED ${SOURCES6_CPP})

TARGET_LINK_LIBRARIES(stage
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vlevpz
  vleoov
  vledevs
  vleextension
  vlemanager
  vletranslator)

INSTALL(TARGETS stage
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

INSTALL(FILES moore.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")
INSTALL(FILES moore2.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")
INSTALL(FILES mealy.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")
INSTALL(FILES mealy2.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")
INSTALL(FILES statechart.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")
INSTALL(FILES statechart2.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")
INSTALL(FILES statechart3.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")
INSTALL(FILES statechart4.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")
INSTALL(FILES statechart5.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")
INSTALL(FILES statechart6.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")
INSTALL(FILES statechart7.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")
INSTALL(FILES fddevs.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")
INSTALL(FILES stage.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES ${SOURCES1_CPP} ${SOURCES2_CPP} ${SOURCES3_CPP}
  	      ${SOURCES4_CPP} ${SOURCES5_CPP} ${SOURCES6_CPP} DESTINATION
  "${VLE_SHARE_DIRS}/examples/sources/fsa")
