INCLUDE_DIRECTORIES(
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${CAIROMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR}
  ${CAIROMM_LIBRARY_DIR})

SET(OOV_CPP LocalStreamReader.cpp NetStreamReader.cpp OOV.cpp OutputMatrix.cpp
  Plugin.cpp PluginFactory.cpp SimpleFile.cpp StreamReader.cpp)

SET(OOV_HPP LocalStreamReader.hpp NetStreamReader.hpp OOV.hpp OutputMatrix.hpp
  Plugin.hpp PluginFactory.hpp SimpleFile.hpp StreamReader.hpp)

IF (HAVE_CAIRO)
 SET(OOV_HPP ${OOV_HPP} CairoPlugin.hpp)
 SET(OOV_CPP ${OOV_CPP} CairoPlugin.cpp)
ENDIF (HAVE_CAIRO)

ADD_LIBRARY(vleoov SHARED ${OOV_CPP};${OOV_HPP})

SET_TARGET_PROPERTIES(vleoov PROPERTIES ${VLE_LIBRARY_PROPERTIES})

TARGET_LINK_LIBRARIES(vleoov
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${CAIROMM_LIBRARIES}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph)

INSTALL(TARGETS vleoov
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

INSTALL(FILES ${OOV_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/oov)

ADD_SUBDIRECTORY(plugins)
