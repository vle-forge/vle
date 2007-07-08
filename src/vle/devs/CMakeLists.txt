INCLUDE_DIRECTORIES(
	${VLE_SOURCE_DIR}/src
	${GLIBMM_INCLUDE_DIRS}
	${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
	${GLIBMM_LIBRARY_DIRS}
	${LIBXMLPP_LIBRARY_DIR})

SET(DEVS_CPP
    Coordinator.cpp Dynamics.cpp Event.cpp EventObserver.cpp EventTable.cpp
    ExternalEvent.cpp InitEvent.cpp InstantaneousEvent.cpp InternalEvent.cpp
    Measure.cpp ModelFactory.cpp Observer.cpp RootCoordinator.cpp Simulator.cpp
    StateEvent.cpp Stream.cpp Time.cpp TimedObserver.cpp)

SET(DEVS_HPP
    Attribute.hpp Coordinator.hpp DevsTypes.hpp Dynamics.hpp Event.hpp
    EventList.hpp EventObserver.hpp EventTable.hpp Executive.hpp
    ExternalEvent.hpp ExternalEventList.hpp InitEvent.hpp InstantaneousEvent.hpp
    InstantaneousEventList.hpp InternalEvent.hpp Measure.hpp ModelFactory.hpp
    Observer.hpp RootCoordinator.hpp Simulator.hpp StateEvent.hpp Stream.hpp
    TimedObserver.hpp Time.hpp)

ADD_LIBRARY(vledevs SHARED ${DEVS_CPP};${DEVS_HPP})

INSTALL(TARGETS vledevs DESTINATION ${VLE_LIBRARY_DIR})
INSTALL(FILES ${DEVS_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/devs)
