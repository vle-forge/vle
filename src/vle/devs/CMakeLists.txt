add_sources(vlelib Coordinator.cpp Dynamics.cpp DynamicsDbg.cpp
  DynamicsWrapper.cpp Executive.cpp ExternalEvent.cpp
  ExternalEventList.cpp InitEventList.cpp InternalEvent.cpp ModelFactory.cpp
  RootCoordinator.cpp Scheduler.cpp Simulator.cpp Time.cpp View.cpp
  ViewEvent.cpp)

install(FILES Dynamics.hpp DynamicsWrapper.hpp Executive.hpp
  ExternalEvent.hpp ExternalEventList.hpp InitEventList.hpp
  ObservationEvent.hpp Time.hpp DESTINATION ${VLE_INCLUDE_DIRS}/devs)

add_subdirectory(test)
