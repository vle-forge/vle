add_executable(test_graph graph.cpp ../../utils/Filesystem.cpp
  ../../utils/ContextModule.cpp ../../devs/ModelFactory.cpp
  ../../devs/Simulator.cpp ../../devs/Coordinator.cpp
  ../../devs/RootCoordinator.cpp ../../devs/Scheduler.cpp
  ../../devs/View.cpp ../../devs/DynamicsDbg.cpp ../../devs/Dynamics.cpp
  ../MatrixTranslator.cpp ../GraphTranslator.cpp)

set_target_properties(test_graph PROPERTIES
  COMPILE_DEFINITIONS
  TRANSLATOR_TEST_DIR=\"${CMAKE_SOURCE_DIR}/src/vle/translator/test\")

target_link_libraries(test_graph vlelib ${CMAKE_THREAD_LIBS_INIT})
add_test(translatorgraph test_graph)
