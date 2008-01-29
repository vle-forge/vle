INCLUDE_DIRECTORIES(
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${CAIROMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS})

ADD_DEFINITIONS(
  -DVLE_PACKAGE_VERSION="\\"${VLE_NAME_COMPLETE}\\"")

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR}
  ${CAIROMM_LIBRARY_DIR})

SET(OOV_CPP LocalStreamReader.cpp NetStreamReader.cpp OOV.cpp
  OovLocalStreamReader.cpp OovNetStreamReader.cpp Plugin.cpp SimpleFile.cpp
  StreamReader.cpp)

SET(OOV_HPP LocalStreamReader.hpp NetStreamReader.hpp OOV.hpp
  OovLocalStreamReader.hpp OovNetStreamReader.hpp Plugin.hpp SimpleFile.hpp
  StreamReader.hpp)

IF (HAVE_CAIRO)
 SET(OOV_HPP ${OOV_HPP} CairoPlugin.hpp)
ENDIF (HAVE_CAIRO)

ADD_LIBRARY(vleoov SHARED ${OOV_CPP};${OOV_HPP})

TARGET_LINK_LIBRARIES(vleoov
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${CAIROMM_LIBRARIES}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vlevpz
  vleoov)

INSTALL(TARGETS vleoov
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

INSTALL(FILES ${OOV_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/oov)

ADD_SUBDIRECTORY(plugins)
