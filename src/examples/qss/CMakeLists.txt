INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/qss)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

SET(SOURCES_CPP ladybirdplantlouse.cpp ladybirdplantlouse.hpp plantlouse.cpp
  plantlouse.hpp ladybird.hpp ladybird.cpp smartgardener.cpp smartgardener.hpp
  plantlouse2.cpp plantlouse2.hpp ladybird2.hpp ladybird2.cpp smartgardener2.cpp
  smartgardener2.hpp)

ADD_LIBRARY(ladybirdplantlouse SHARED ${SOURCES_CPP})

TARGET_LINK_LIBRARIES(ladybirdplantlouse
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

INSTALL(TARGETS ladybirdplantlouse
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

INSTALL(FILES qss_exp1_txt.vpz qss_exp1_test.vpz qss_exp1.vpz
  qss_exp2_txt.vpz qss_exp2.vpz qss_exp3_txt.vpz qss_exp3_test.vpz
   qss_exp3.vpz qss_exp4_txt.vpz qss_exp4.vpz qss_exp5.vpz
  qss_exp6.vpz qss_exp7.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES ${SOURCES_CPP} DESTINATION
  "${VLE_SHARE_DIRS}/examples/sources/qss")
