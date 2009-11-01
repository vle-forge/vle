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
    COMMAND /usr/bin/i586-mingw32msvc-windres
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/vfl.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
  SET(OS_SPECIFIC_PATH_IMPL ${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
ELSE(WIN32)
  SET(OS_SPECIFIC_PATH_IMPL)
ENDIF(WIN32)


SET(DEVS_CPP Coordinator.cpp Dynamics.cpp Event.cpp EventTable.cpp EventView.cpp
  ExternalEvent.cpp Executive.cpp RequestEvent.cpp FinishView.cpp
  InternalEvent.cpp LocalStreamWriter.cpp ModelFactory.cpp NetStreamWriter.cpp
  Observable.cpp Pools.cpp RootCoordinator.cpp Simulator.cpp
  ObservationEvent.cpp StreamWriter.cpp Time.cpp TimedView.cpp View.cpp
  ${OS_SPECIFIC_PATH_IMPL})

SET(DEVS_HPP Attribute.hpp Coordinator.hpp Dynamics.hpp DllDefines.hpp
  DynamicsWrapper.hpp Event.hpp EventList.hpp EventView.hpp EventTable.hpp
  Executive.hpp ExternalEvent.hpp ExternalEventList.hpp FinishView.hpp
  RequestEvent.hpp RequestEventList.hpp InternalEvent.hpp LocalStreamWriter.hpp
  ModelFactory.hpp NetStreamWriter.hpp Observable.hpp Pools.hpp
  RootCoordinator.hpp Simulator.hpp ObservationEvent.hpp StreamWriter.hpp
  TimedView.hpp Time.hpp View.hpp)

ADD_LIBRARY(vledevs SHARED ${DEVS_CPP};${DEVS_HPP})

SET_TARGET_PROPERTIES(vledevs PROPERTIES ${VLE_LIBRARY_PROPERTIES})
SET_TARGET_PROPERTIES(vledevs PROPERTIES OUTPUT_NAME
  "vledevs-${VLE_VERSION_SHORT}")

TARGET_LINK_LIBRARIES(vledevs
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${OS_SPECIFIC_LIBRARIES}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vleoov
  vlevpz)

INSTALL(TARGETS vledevs
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

INSTALL(FILES ${DEVS_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/devs)

IF (HAVE_UNITTESTFRAMEWORK)
  ADD_SUBDIRECTORY(test)
ENDIF (HAVE_UNITTESTFRAMEWORK)
