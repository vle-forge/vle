INCLUDE_DIRECTORIES(
	${VLE_SOURCE_DIR}/src
	${GLIBMM_INCLUDE_DIRS}
	${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
	${GLIBMM_LIBRARY_DIRS}
	${LIBXMLPP_LIBRARY_DIR})

SET(OOV_CPP
    LocalStreamReader.cpp Plugin.cpp StreamReader.cpp)

SET(OOV_HPP
    oov.hpp LocalStreamReader.hpp NetStreamReader.hpp Plugin.hpp
    StreamReader.hpp)

ADD_LIBRARY(vleoov SHARED ${OOV_CPP};${OOV_HPP})

TARGET_LINK_LIBRARIES(vleoov
	${GLIBMM_LIBRARIES}
	${LIBXMLPP_LIBRARIES}
	${GTHREAD_LIBRARIES}
	vlegeometry
	vleutils
	vledata
	vlevalue
    	vlegraph
        vlevpz
	vleoov)

INSTALL(TARGETS vleoov DESTINATION ${VLE_LIBRARY_DIR})
INSTALL(FILES ${OOV_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/oov)

ADD_SUBDIRECTORY(plugins)
