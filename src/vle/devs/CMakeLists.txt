INCLUDE_DIRECTORIES(
  ${VLE_SOURCE_DIR}/src
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR})

SET(DEVS_CPP Coordinator.cpp Dynamics.cpp Event.cpp EventTable.cpp
  EventView.cpp ExternalEvent.cpp InitEvent.cpp InstantaneousEvent.cpp
  InternalEvent.cpp LocalStreamWriter.cpp ModelFactory.cpp NetStreamWriter.cpp
  Observable.cpp RootCoordinator.cpp Simulator.cpp StateEvent.cpp
  StreamWriter.cpp Time.cpp TimedView.cpp View.cpp)

SET(DEVS_HPP Attribute.hpp Coordinator.hpp DevsTypes.hpp Dynamics.hpp
  DynamicsWrapper.hpp Event.hpp EventList.hpp EventView.hpp EventTable.hpp
  Executive.hpp ExternalEvent.hpp ExternalEventList.hpp InitEvent.hpp
  InstantaneousEvent.hpp InstantaneousEventList.hpp InternalEvent.hpp
  LocalStreamWriter.hpp ModelFactory.hpp NetStreamWriter.hpp Observable.hpp
  RootCoordinator.hpp Simulator.hpp StateEvent.hpp StreamWriter.hpp
  TimedView.hpp Time.hpp View.hpp)

ADD_LIBRARY(vledevs SHARED ${DEVS_CPP};${DEVS_HPP})

TARGET_LINK_LIBRARIES(vledevs
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vlegeometry
  vleutils
  vledata
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
