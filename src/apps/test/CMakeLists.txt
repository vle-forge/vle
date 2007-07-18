INCLUDE_DIRECTORIES(
	${VLE_SOURCE_DIR}/src
	${GLIBMM_INCLUDE_DIRS}
	${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
	${VLE_SOURCE_DIR}/src/vle/utils
	${GLIBMM_LIBRARY_DIRS}
	${LIBXMLPP_LIBRARY_DIRS})

ADD_EXECUTABLE(vlemodeltest modeltest.cpp)

ADD_EXECUTABLE(vletranslatortest translatortest.cpp)

TARGET_LINK_LIBRARIES(vletranslatortest
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

INSTALL(TARGETS vlemodeltest DESTINATION ${VLE_BIN_DIR})
INSTALL(TARGETS vletranslatortest DESTINATION ${VLE_BIN_DIR})
INSTALL(FILES vlemodeltest.1 DESTINATION ${VLE_MANPAGE_PATH})
INSTALL(FILES vletranslatortest.1 DESTINATION ${VLE_MANPAGE_PATH})
