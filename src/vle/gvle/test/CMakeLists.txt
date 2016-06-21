add_executable(test_dom test_dom.cpp)
target_link_libraries(test_dom vlelib gvlelib ${CMAKE_THREAD_LIBS_INIT})
add_test(test_dom test_dom)
