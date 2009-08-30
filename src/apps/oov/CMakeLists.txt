INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIRS})

IF (WIN32)
  ADD_EXECUTABLE(oov main.cpp OptionGroup.cpp OptionGroup.hpp)
ELSE (WIN32)
  ADD_EXECUTABLE(oov main.cpp OptionGroup.cpp OptionGroup.hpp)
  SET_TARGET_PROPERTIES(oov PROPERTIES ${VLE_APP_PROPERTIES})
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(oov
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vleoov
  vlevpz
  vledevs
  vleextension
  vlemanager
  vletranslator)

INSTALL(TARGETS oov DESTINATION bin)
INSTALL(FILES oov.1 DESTINATION ${VLE_MANPAGE_PATH})
