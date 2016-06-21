add_executable(test_manager test1.cpp)
target_link_libraries(test_manager vlelib ${CMAKE_THREAD_LIBS_INIT})
add_test(manager_test test_manager)
