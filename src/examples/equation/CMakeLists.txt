INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/lifegame)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

ADD_LIBRARY(equation1 SHARED A1.cpp A1.hpp B1.cpp B1.hpp C1.cpp C1.hpp
		      D1.cpp D1.hpp)

TARGET_LINK_LIBRARIES(equation1
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

ADD_LIBRARY(equation2 SHARED A2.cpp A2.hpp B2.cpp B2.hpp C2.cpp C2.hpp
		      D2.cpp D2.hpp)

TARGET_LINK_LIBRARIES(equation2
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

INSTALL(TARGETS equation1
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

INSTALL(TARGETS equation2
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

INSTALL(FILES equation1.vpz
  DESTINATION "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES equation2.vpz
  DESTINATION "${VLE_SHARE_DIRS}/examples")
