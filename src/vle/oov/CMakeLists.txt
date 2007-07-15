INCLUDE_DIRECTORIES(
	${VLE_SOURCE_DIR}/src
	${GLIBMM_INCLUDE_DIRS}
	${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
	${GLIBMM_LIBRARY_DIRS}
	${LIBXMLPP_LIBRARY_DIR})

SET(OOV_CPP
    DelObservableTrame.cpp NewObservableTrame.cpp ParameterTrame.cpp Plugin.cpp
    StreamReader.cpp Trame.cpp ValueTrame.cpp)

SET(OOV_HPP
    DelObservableTrame.hpp NewObservableTrame.hpp ParameterTrame.hpp
    Plugin.hpp StreamReader.hpp Trame.hpp ValueTrame.hpp)

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
	vledevs)

INSTALL(TARGETS vleoov DESTINATION ${VLE_LIBRARY_DIR})
INSTALL(FILES ${OOV_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/oov)
