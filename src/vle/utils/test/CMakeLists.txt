add_executable(test_algo test1.cpp)
target_link_libraries(test_algo vlelib ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_template test_template.cpp)
target_link_libraries(test_template vlelib ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_parser test_parser.cpp)
target_link_libraries(test_parser vlelib ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_package test_package.cpp)
target_link_libraries(test_package vlelib ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(test_package PROPERTIES
  COMPILE_DEFINITIONS UTILS_TEST_DIR=\"${CMAKE_SOURCE_DIR}/share/template\")

add_executable(test_downloadmanager test_downloadmanager.cpp)
target_link_libraries(test_downloadmanager vlelib ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_shell test_shell.cpp)
target_link_libraries(test_shell vlelib ${CMAKE_THREAD_LIBS_INIT})

add_test(utilstest_algo test_algo)
add_test(utilstest_template test_template)
add_test(utilstest_parser test_parser)
add_test(utilstest_package test_package)
add_test(utilstest_downloadmanager test_downloadmanager)
add_test(utilstest_shell test_shell)
