add_sources(vlelib Coordinator.cpp Dynamics.cpp DynamicsDbg.cpp
  Executive.cpp ExternalEvent.cpp ExternalEventList.cpp
  InternalEvent.cpp ModelFactory.cpp RootCoordinator.cpp Scheduler.cpp
  Simulator.cpp StreamWriter.cpp Time.cpp View.cpp ViewEvent.cpp)

install(FILES Dynamics.hpp DynamicsWrapper.hpp
  ExecutiveDbg.hpp Executive.hpp ExternalEvent.hpp
  ExternalEventList.hpp InitEventList.hpp ObservationEvent.hpp
  Time.hpp DESTINATION ${VLE_INCLUDE_DIRS}/devs)

if (VLE_HAVE_UNITTESTFRAMEWORK)
  add_subdirectory(test)
endif ()
