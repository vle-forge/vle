INCLUDE_DIRECTORIES(
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR})

SET(VPZ_CPP Class.cpp Classes.cpp Condition.cpp Conditions.cpp
  DelObservableTrame.cpp Dynamic.cpp Dynamics.cpp EndTrame.cpp Experiment.cpp
  Model.cpp NewObservableTrame.cpp Observable.cpp Observables.cpp Output.cpp
  Outputs.cpp ParameterTrame.cpp Project.cpp RefreshTrame.cpp Replicas.cpp
  SaxStackValue.cpp SaxStackVpz.cpp Trame.cpp ValueTrame.cpp View.cpp Views.cpp
  Vpz.cpp SaxParser.cpp)

SET(VPZ_HPP Base.hpp Classes.hpp Class.hpp Condition.hpp Conditions.hpp
  DelObservableTrame.hpp Dynamic.hpp Dynamics.hpp EndTrame.hpp Experiment.hpp
  Model.hpp NewObservableTrame.hpp Observable.hpp Observables.hpp Output.hpp
  Outputs.hpp ParameterTrame.hpp Port.hpp Project.hpp RefreshTrame.hpp
  Replicas.hpp SaxStackValue.hpp SaxStackVpz.hpp Structures.hpp Trame.hpp
  ValueTrame.hpp View.hpp Views.hpp Vpz.hpp SaxParser.hpp)

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

INSTALL(TARGETS vlevpz
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

INSTALL(FILES ${VPZ_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/vpz)

IF (HAVE_UNITTESTFRAMEWORK)
  ADD_SUBDIRECTORY(test)
ENDIF (HAVE_UNITTESTFRAMEWORK)
