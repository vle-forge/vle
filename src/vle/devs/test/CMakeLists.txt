add_executable(test_time time.cpp)
target_link_libraries(test_time vlelib ${CMAKE_THREAD_LIBS_INIT})
add_test(devsttime test_time)

add_executable(test_coordinator coordinator.cpp ../DynamicsDbg.cpp
  ../../utils/Filesystem.cpp ../../utils/ContextModule.cpp
  ../ModelFactory.cpp ../Simulator.cpp ../Coordinator.cpp
  ../RootCoordinator.cpp ../Scheduler.cpp ../View.cpp)

target_link_libraries(test_coordinator vlelib ${CMAKE_THREAD_LIBS_INIT})
add_test(devscoordinator test_coordinator)

add_executable(test_mdl mdl.cpp ../../utils/Filesystem.cpp
  ../../utils/ContextModule.cpp ../DynamicsDbg.cpp ../ModelFactory.cpp
  ../Simulator.cpp ../Coordinator.cpp ../RootCoordinator.cpp
  ../Scheduler.cpp ../View.cpp)

set_target_properties(test_mdl PROPERTIES
  COMPILE_DEFINITIONS DEVS_TEST_DIR=\"${CMAKE_SOURCE_DIR}/src/vle/devs/test\")

target_link_libraries(test_mdl vlelib ${CMAKE_THREAD_LIBS_INIT})
add_test(devsmdl test_mdl)
