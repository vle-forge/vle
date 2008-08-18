INCLUDE_DIRECTORIES(
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${VLE_SOURCE_DIR}/src/vle/utils
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIRS})

ADD_EXECUTABLE(vlemodeltest modeltest.cpp)

SET_TARGET_PROPERTIES(vlemodeltest PROPERTIES ${VLE_APP_PROPERTIES})

TARGET_LINK_LIBRARIES(vlemodeltest
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
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
