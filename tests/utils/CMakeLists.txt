vle_declare_test(test_algo test1.cpp)
vle_declare_test(test_template test_template.cpp)
vle_declare_test(test_parser test_parser.cpp)
vle_declare_test(test_downloadmanager test_downloadmanager.cpp)
# vle_declare_test(test_shell test_shell.cpp)
vle_declare_test(test_package test_package.cpp)

set_target_properties(test_package PROPERTIES
  COMPILE_DEFINITIONS UTILS_TEST_DIR=\"${CMAKE_SOURCE_DIR}/share/template\")
