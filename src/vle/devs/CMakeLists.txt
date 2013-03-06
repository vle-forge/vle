add_sources(vlelib Attribute.hpp Coordinator.cpp Coordinator.hpp
  Dynamics.cpp DynamicsDbg.cpp DynamicsDbg.hpp Dynamics.hpp
  DynamicsWrapper.hpp EventTable.cpp EventTable.hpp Executive.cpp
  ExecutiveDbg.hpp Executive.hpp ExternalEvent.cpp ExternalEvent.hpp
  ExternalEventList.cpp ExternalEventList.hpp InitEventList.hpp
  InternalEvent.cpp InternalEvent.hpp ModelFactory.cpp
  ModelFactory.hpp ObservationEvent.cpp ObservationEvent.hpp
  RootCoordinator.cpp RootCoordinator.hpp Simulator.cpp Simulator.hpp
  StreamWriter.cpp StreamWriter.hpp Time.cpp Time.hpp View.cpp
  ViewEvent.hpp View.hpp)

install(FILES Attribute.hpp Coordinator.hpp DynamicsDbg.hpp
  Dynamics.hpp DynamicsWrapper.hpp EventTable.hpp ExecutiveDbg.hpp
  Executive.hpp ExternalEvent.hpp ExternalEventList.hpp
  InitEventList.hpp InternalEvent.hpp ModelFactory.hpp
  ObservationEvent.hpp RootCoordinator.hpp Simulator.hpp
  StreamWriter.hpp Time.hpp ViewEvent.hpp View.hpp DESTINATION
  ${VLE_INCLUDE_DIRS}/devs)

if (VLE_HAVE_UNITTESTFRAMEWORK)
  add_subdirectory(test)
endif ()
