INCLUDE_DIRECTORIES(
	${VLE_SOURCE_DIR}/src
	${GLIBMM_INCLUDE_DIRS}
	${LIBXMLPP_INCLUDE_DIRS})

ADD_DEFINITIONS(
	-DVLE_PREFIX_DIR="\\"${CMAKE_INSTALL_PREFIX}\\""
	-DVLE_PACKAGE_NAME="\\"${VLE_PACKAGE_NAME}\\""
	-DVLE_PACKAGE_VERSION="\\"${VLE_PACKAGE_VERSION}\\"")

LINK_DIRECTORIES(
	${GLIBMM_LIBRARY_DIRS}
	${LIBXMLPP_LIBRARY_DIR})

SET(MANAGER_CPP
    ExperimentGenerator.cpp LinearExperimentGenerator.cpp Manager.cpp
    Simulator.cpp TotalExperimentGenerator.cpp VLE.cpp)

SET(MANAGER_HPP
    ExperimentGenerator.hpp LinearExperimentGenerator.hpp Manager.hpp
    Simulator.hpp TotalExperimentGenerator.hpp VLE.hpp)

ADD_LIBRARY(vlemanager SHARED ${MANAGER_CPP};${MANAGER_HPP})

INSTALL(TARGETS vlemanager DESTINATION ${VLE_LIBRARY_DIR})
INSTALL(FILES ${MANAGER_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/manager)
