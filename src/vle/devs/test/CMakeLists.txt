add_executable(test_time time.cpp)
target_link_libraries(test_time vlelib ${Boost_LIBRARIES})
add_test(devsttime test_time)

add_executable(test_coordinator coordinator.cpp ../ModelFactory.cpp
  ../Simulator.cpp ../Coordinator.cpp ../RootCoordinator.cpp
  ../Scheduler.cpp ../StreamWriter.cpp ../View.cpp)

target_link_libraries(test_coordinator vlelib ${Boost_LIBRARIES})

add_test(devscoordinator test_coordinator)
