INCLUDE_DIRECTORIES(
	${VLE_SOURCE_DIR}/src
	${GLIBMM_INCLUDE_DIRS}
	${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
	${GLIBMM_LIBRARY_DIRS}
	${LIBXMLPP_LIBRARY_DIR})

SET(GRAPH_CPP
    AtomicModel.cpp CoupledModel.cpp Model.cpp ModelPort.cpp ModelPortList.cpp
    NoVLEModel.cpp)

SET(GRAPH_HPP
    AtomicModel.hpp CoupledModel.hpp graph.hpp Model.hpp ModelPort.hpp
    ModelPortList.hpp NoVLEModel.hpp)

ADD_LIBRARY(vlegraph SHARED ${GRAPH_CPP};${GRAPH_HPP})

INSTALL(TARGETS vlegraph DESTINATION ${VLE_LIBRARY_DIR})
INSTALL(FILES ${GRAPH_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/graph)
