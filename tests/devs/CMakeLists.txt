vle_declare_test(test_time time.cpp)
# vle_declare_test(test_coordinator coordinator.cpp)
# vle_declare_test(test_mdl mdl.cpp)
vle_declare_test(test_multicomponant component.cpp)

set_target_properties(test_multicomponant PROPERTIES
  COMPILE_DEFINITIONS DEVS_TEST_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
