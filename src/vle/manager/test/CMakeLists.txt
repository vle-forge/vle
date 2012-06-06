add_executable(test_manager test1.cpp)

target_link_libraries(test_manager vlelib ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(manager_test test_manager)
