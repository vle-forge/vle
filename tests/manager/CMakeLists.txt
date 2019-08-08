vle_declare_test(test_manager test1.cpp)
vle_declare_test(test_accumulators test_accumulators.cpp)

target_include_directories(test_accumulators
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/vle/manager>)
