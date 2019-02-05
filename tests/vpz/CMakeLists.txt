vle_declare_test(test_vpz_values test1.cpp)
vle_declare_test(test_vpz_project test2.cpp)
vle_declare_test(test_vpz_translator test3.cpp)
vle_declare_test(test_vpz_io test4.cpp)
set_target_properties(test_vpz_io PROPERTIES
  COMPILE_DEFINITIONS VPZ_TEST_DIR=\"${CMAKE_SOURCE_DIR}/share/template\")

vle_declare_test(test_vpz_oov test5.cpp)
vle_declare_test(test_vpz_classes test6.cpp)
vle_declare_test(test_vpz_structures test7.cpp)
vle_declare_test(test_vpz_graph test8.cpp)
set_target_properties(test_vpz_graph PROPERTIES
  COMPILE_DEFINITIONS VPZ_TEST_DIR=\"${CMAKE_SOURCE_DIR}/share/template\")
