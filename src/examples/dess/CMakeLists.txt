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

SET(SOURCES_CPP Counter.cpp Counter.hpp E.cpp E.hpp I2.cpp I2.hpp I3.cpp I3.hpp
  I.cpp I.hpp R2.cpp R2.hpp R3.cpp R3.hpp R.cpp R.hpp S2.cpp S2.hpp S3.cpp
  S3.hpp S.cpp seir.cpp seir.hpp S.hpp sir.cpp sir.hpp)

ADD_LIBRARY(sir SHARED ${SOURCES_CPP})

TARGET_LINK_LIBRARIES(sir
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

INSTALL(TARGETS sir
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

INSTALL(FILES sir.vpz sir2.vpz sir3.vpz seir.vpz seir2.vpz
  DESTINATION "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES ${SOURCES_CPP} DESTINATION
  "${VLE_SHARE_DIRS}/examples/sources/dess")
