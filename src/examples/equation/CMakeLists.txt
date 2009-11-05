INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/equation)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

SET(SOURCES1_CPP A1.cpp A1.hpp B1.cpp B1.hpp C1.cpp C1.hpp D1.cpp D1.hpp sum.cpp
  Perturb.cpp B3.cpp C3.cpp D3.cpp)

ADD_LIBRARY(equation1 SHARED ${SOURCES1_CPP})

TARGET_LINK_LIBRARIES(equation1
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

SET(SOURCES2_CPP A2.cpp A2.hpp B2.cpp B2.hpp C2.cpp C2.hpp D2.cpp D2.hpp)

ADD_LIBRARY(equation2 SHARED ${SOURCES2_CPP})

TARGET_LINK_LIBRARIES(equation2
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

SET(SOURCES3_CPP E1.cpp E1.hpp F1.hpp F1.cpp G1.hpp G1.cpp E2.cpp E2.hpp)

ADD_LIBRARY(equation3 SHARED ${SOURCES3_CPP})

TARGET_LINK_LIBRARIES(equation3
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

INSTALL(TARGETS equation1
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

INSTALL(TARGETS equation2
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

INSTALL(TARGETS equation3
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

INSTALL(FILES equation1.vpz equation2.vpz equation3.vpz equation4.vpz
  equation5.vpz equation6.vpz equation7.vpz equation8.vpz equation9.vpz
  equation10.vpz equation11.vpz equation12.vpz equation13.vpz
  DESTINATION "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES ${SOURCES1_CPP} ${SOURCES2_CPP} ${SOURCES3_CPP} DESTINATION
  "${VLE_SHARE_DIRS}/examples/sources/equation")
