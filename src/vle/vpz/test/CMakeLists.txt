ADD_EXECUTABLE(test_vpz_values test1.cpp)
TARGET_LINK_LIBRARIES(test_vpz_values vlelib ${CMAKE_THREAD_LIBS_INIT})

ADD_EXECUTABLE(test_vpz_project test2.cpp)
TARGET_LINK_LIBRARIES(test_vpz_project vlelib ${CMAKE_THREAD_LIBS_INIT})

ADD_EXECUTABLE(test_vpz_translator test3.cpp)
TARGET_LINK_LIBRARIES(test_vpz_translator vlelib ${CMAKE_THREAD_LIBS_INIT})

ADD_EXECUTABLE(test_vpz_io test4.cpp)
TARGET_LINK_LIBRARIES(test_vpz_io vlelib ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(test_vpz_io PROPERTIES
  COMPILE_DEFINITIONS VPZ_TEST_DIR=\"${CMAKE_SOURCE_DIR}/share/template\")

ADD_EXECUTABLE(test_vpz_oov test5.cpp)
TARGET_LINK_LIBRARIES(test_vpz_oov vlelib ${CMAKE_THREAD_LIBS_INIT})

ADD_EXECUTABLE(test_vpz_classes test6.cpp)
TARGET_LINK_LIBRARIES(test_vpz_classes vlelib ${CMAKE_THREAD_LIBS_INIT})

ADD_EXECUTABLE(test_vpz_structures test7.cpp)
TARGET_LINK_LIBRARIES(test_vpz_structures vlelib ${CMAKE_THREAD_LIBS_INIT})

ADD_EXECUTABLE(test_vpz_graph test8.cpp)
TARGET_LINK_LIBRARIES(test_vpz_graph vlelib ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(test_vpz_graph PROPERTIES
  COMPILE_DEFINITIONS VPZ_TEST_DIR=\"${CMAKE_SOURCE_DIR}/share/template\")

ADD_TEST(vpztest_values test_vpz_values)
ADD_TEST(vpztest_project test_vpz_project)
ADD_TEST(vpztest_translator test_vpz_translator)
ADD_TEST(vpztest_io test_vpz_io)
ADD_TEST(vpztest_oov test_vpz_oov)
ADD_TEST(vpztest_classes test_vpz_classes)
ADD_TEST(vpztest_structures test_vpz_structures)
ADD_TEST(vpztest_graph test_vpz_graph)
