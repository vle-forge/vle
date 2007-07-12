INCLUDE_DIRECTORIES(
	${VLE_SOURCE_DIR}/src
	${GLIBMM_INCLUDE_DIRS}
	${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
	${GLIBMM_LIBRARY_DIRS}
	${LIBXMLPP_LIBRARY_DIR})

SET(DEVS_CPP
    Coordinator.cpp Dynamics.cpp Event.cpp EventTable.cpp EventView.hpp
    ExternalEvent.cpp InitEvent.cpp InstantaneousEvent.cpp InternalEvent.cpp
    ModelFactory.cpp Observable.cpp RootCoordinator.cpp Simulator.cpp
    StateEvent.cpp Stream.cpp Time.cpp TimedView.cpp View.cpp)

SET(DEVS_HPP
    Attribute.hpp Coordinator.hpp DevsTypes.hpp Dynamics.hpp Event.hpp
    EventList.hpp EventView.hpp EventTable.hpp Executive.hpp ExternalEvent.hpp
    ExternalEventList.hpp InitEvent.hpp InstantaneousEvent.hpp
    InstantaneousEventList.hpp InternalEvent.hpp ModelFactory.hpp
    Observable.hpp RootCoordinator.hpp Simulator.hpp StateEvent.hpp Stream.hpp
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
	vlevpz)

INSTALL(TARGETS vledevs DESTINATION ${VLE_LIBRARY_DIR})
INSTALL(FILES ${DEVS_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/devs)
