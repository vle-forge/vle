INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/gens)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

ADD_LIBRARY(gens SHARED Beep.cpp Beep.hpp Counter.cpp Counter.hpp
  GenExecutive.cpp GenExecutive.hpp GenExecutiveClass.cpp GenExecutiveClass.hpp)

TARGET_LINK_LIBRARIES(gens
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

INSTALL(TARGETS gens
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

INSTALL(FILES gens.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")
INSTALL(FILES genswithclass.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")
