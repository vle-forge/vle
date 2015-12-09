add_executable(test_dom test_dom.cpp)
target_link_libraries(test_dom vlelib gvlelib 
   ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
add_test(test_dom test_dom)
