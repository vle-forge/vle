add_executable(test_algo test1.cpp)

target_link_libraries(test_algo vlelib ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(test_template test_template.cpp)

target_link_libraries(test_template vlelib ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(test_parser test_parser.cpp)

target_link_libraries(test_parser vlelib ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(test_package test_package.cpp)

target_link_libraries(test_package vlelib ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY})

add_executable(test_downloadmanager test_downloadmanager.cpp)

target_link_libraries(test_downloadmanager vlelib ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(utilstest_algo test_algo)
add_test(utilstest_template test_template)
add_test(utilstest_parser test_parser)
add_test(utilstest_package test_package)
add_test(utilstest_downloadmanager test_downloadmanager)
