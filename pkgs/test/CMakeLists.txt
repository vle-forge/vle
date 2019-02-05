
add_executable(test_vle_output test_vle_output.cpp 
  ${CMAKE_SOURCE_DIR}/src/vle/devs/Dynamics.cpp
  ${CMAKE_SOURCE_DIR}/src/vle/devs/DynamicsDbg.cpp
  ${CMAKE_SOURCE_DIR}/src/vle/devs/RootCoordinator.cpp
  ${CMAKE_SOURCE_DIR}/src/vle/devs/Coordinator.cpp
  ${CMAKE_SOURCE_DIR}/src/vle/devs/View.cpp
  ${CMAKE_SOURCE_DIR}/src/vle/devs/Simulator.cpp
  ${CMAKE_SOURCE_DIR}/src/vle/devs/Scheduler.cpp
  ${CMAKE_SOURCE_DIR}/src/vle/devs/ModelFactory.cpp)

target_include_directories(test_vle_output PUBLIC
  ${CMAKE_SOURCE_DIR}/src ${VLE_BINARY_DIR}/src/
  ${Boost_INCLUDE_DIRS}
)

set_target_properties(test_vle_output PROPERTIES
  COMPILE_DEFINITIONS PKGS_TEST_DIR=\"${CMAKE_SOURCE_DIR}/src/pkgs/test\")

target_link_libraries(test_vle_output vlelib ${CMAKE_THREAD_LIBS_INIT})
add_test(test_vle_output test_vle_output)
