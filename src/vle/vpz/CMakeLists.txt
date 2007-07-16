INCLUDE_DIRECTORIES(
	${VLE_SOURCE_DIR}/src
	${GLIBMM_INCLUDE_DIRS}
	${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
	${GLIBMM_LIBRARY_DIRS}
	${LIBXMLPP_LIBRARY_DIR})

SET(VPZ_CPP 
    Class.cpp Classes.cpp Condition.cpp Conditions.cpp Dynamic.cpp Dynamics.cpp
    EOV.cpp Experiment.cpp Model.cpp NoVLE.cpp NoVLEs.cpp Observable.cpp
    Observables.cpp Output.cpp Outputs.cpp Project.cpp Replicas.cpp
    SaxStackValue.cpp SaxVPZ.cpp Translator.cpp View.cpp Views.cpp Vpz.cpp)

SET(VPZ_HPP
    Base.hpp Classes.hpp Class.hpp Condition.hpp Conditions.hpp Dynamic.hpp
    Dynamics.hpp EOV.hpp Experiment.hpp Model.hpp NoVLE.hpp NoVLEs.hpp
    Observable.hpp Observables.hpp Output.hpp Outputs.hpp Port.hpp Project.hpp
    Replicas.hpp SaxStackValue.hpp SaxVPZ.hpp Structures.hpp Translator.hpp
    View.hpp Views.hpp Vpz.hpp)

ADD_LIBRARY(vlevpz SHARED ${VPZ_HPP};${VPZ_CPP})

TARGET_LINK_LIBRARIES(vlevpz
	${GLIBMM_LIBRARIES}
	${LIBXMLPP_LIBRARIES}
	${GTHREAD_LIBRARIES}
	vlegeometry
	vleutils
	vledata
	vlevalue
	vlegraph)

INSTALL(TARGETS vlevpz DESTINATION ${VLE_LIBRARY_DIR})
INSTALL(FILES ${VPZ_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/vpz)


ADD_SUBDIRECTORY(test)
