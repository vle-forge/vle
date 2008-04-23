INCLUDE_DIRECTORIES(
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS})

ADD_DEFINITIONS(
  -DVLE_PACKAGE_VERSION="\\"${VLE_NAME_COMPLETE}\\"")

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR})

SET(MANAGER_CPP ExperimentGenerator.cpp JustRun.cpp
  LinearExperimentGenerator.cpp Manager.cpp Run.cpp SimulatorDistant.cpp
  TotalExperimentGenerator.cpp VLE.cpp)

SET(MANAGER_HPP ExperimentGenerator.hpp JustRun.hpp
  LinearExperimentGenerator.hpp Manager.hpp Run.hpp SimulatorDistant.hpp
  TotalExperimentGenerator.hpp Types.hpp VLE.hpp)

ADD_LIBRARY(vlemanager SHARED ${MANAGER_CPP};${MANAGER_HPP})

SET_TARGET_PROPERTIES(vlemanager PROPERTIES VERSION ${VLE_VERSION}
  SOVERSION ${VLE_SOVERSION})

TARGET_LINK_LIBRARIES(vlemanager
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
  vleextension)

INSTALL(TARGETS vlemanager
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

INSTALL(FILES ${MANAGER_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/manager)

IF (HAVE_UNITTESTFRAMEWORK)
  ADD_SUBDIRECTORY(test)
ENDIF (HAVE_UNITTESTFRAMEWORK)
