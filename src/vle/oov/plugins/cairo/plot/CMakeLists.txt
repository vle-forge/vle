INCLUDE_DIRECTORIES(
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS}
  ${CAIROMM_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR}
  ${CAIROMM_LIBRARY_DIRS})

ADD_LIBRARY(cairoplot SHARED Curve.hpp IntCurve.hpp IntCurve.cpp
RealCurve.hpp RealCurve.cpp Parameter.hpp Parameter.cpp Limit.hpp
Plot.cpp Plot.hpp)

TARGET_LINK_LIBRARIES(cairoplot
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${CAIROMM_LIBRARIES}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vleoov)

INSTALL(TARGETS cairoplot
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/stream"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/stream")
