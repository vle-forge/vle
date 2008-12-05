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

ADD_LIBRARY(sir SHARED sir.hpp sir.cpp seir.hpp seir.cpp S.hpp S.cpp
		       I.hpp I.cpp R.hpp R.cpp S2.hpp S2.cpp E.hpp E.cpp
		       I2.hpp I2.cpp R2.hpp R2.cpp S3.hpp S3.cpp I3.cpp
		       I3.hpp R3.hpp R3.cpp Counter.hpp Counter.cpp)

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
