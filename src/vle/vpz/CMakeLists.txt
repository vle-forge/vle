INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR})

IF(WIN32)
  ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vfl.o
    COMMAND ${CMAKE_RC_COMPILER}
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/vfl.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
  SET(OS_SPECIFIC_PATH_IMPL ${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
ELSE(WIN32)
  SET(OS_SPECIFIC_PATH_IMPL)
ENDIF(WIN32)

SET(VPZ_CPP AtomicModels.cpp Class.cpp Classes.cpp Condition.cpp Conditions.cpp
  Dynamic.cpp Dynamics.cpp Experiment.cpp Model.cpp Observable.cpp
  Observables.cpp Output.cpp Outputs.cpp Project.cpp Replicas.cpp
  SaxStackValue.cpp SaxStackVpz.cpp View.cpp Views.cpp Vpz.cpp SaxParser.cpp
  ${OS_SPECIFIC_PATH_IMPL})

SET(VPZ_HPP AtomicModels.hpp Base.hpp Classes.hpp Class.hpp Condition.hpp
  Conditions.hpp DllDefines.hpp Dynamic.hpp Dynamics.hpp Experiment.hpp
  Model.hpp Observable.hpp Observables.hpp Output.hpp Outputs.hpp Port.hpp
  Project.hpp Replicas.hpp SaxStackValue.hpp SaxStackVpz.hpp Structures.hpp
  View.hpp Views.hpp Vpz.hpp SaxParser.hpp)

ADD_LIBRARY(vlevpz SHARED ${VPZ_HPP};${VPZ_CPP})

SET_TARGET_PROPERTIES(vlevpz PROPERTIES ${VLE_LIBRARY_PROPERTIES})
SET_TARGET_PROPERTIES(vlevpz PROPERTIES OUTPUT_NAME
  "vlevpz-${VLE_VERSION_SHORT}")

TARGET_LINK_LIBRARIES(vlevpz
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${OS_SPECIFIC_LIBRARIES}
  vlegeometry
  vleutils
  vlevalue
  vlegraph)

INSTALL(TARGETS vlevpz
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

INSTALL(FILES ${VPZ_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/vpz)

IF (HAVE_UNITTESTFRAMEWORK)
  ADD_SUBDIRECTORY(test)
ENDIF (HAVE_UNITTESTFRAMEWORK)
