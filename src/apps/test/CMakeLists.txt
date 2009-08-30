INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${VLE_SOURCE_DIR}/src/vle/utils
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIRS})

IF (WIN32)
  ADD_EXECUTABLE(vlemodeltest modeltest.cpp)
ELSE (WIN32)
  ADD_EXECUTABLE(vlemodeltest modeltest.cpp)
  SET_TARGET_PROPERTIES(vlemodeltest PROPERTIES ${VLE_APP_PROPERTIES})
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(vlemodeltest
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

INSTALL(TARGETS vlemodeltest DESTINATION bin)
INSTALL(FILES vlemodeltest.1 DESTINATION ${VLE_MANPAGE_PATH})
