INCLUDE_DIRECTORIES(
  ${VLE_SOURCE_DIR}/src
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIRS})

ADD_EXECUTABLE(oov main.cpp OptionGroup.cpp OptionGroup.hpp)

TARGET_LINK_LIBRARIES(oov
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

INSTALL(TARGETS oov DESTINATION bin)
INSTALL(FILES oov.1 DESTINATION ${VLE_MANPAGE_PATH})
