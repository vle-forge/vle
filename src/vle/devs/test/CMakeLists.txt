ADD_LIBRARY(DevsDeleteConnection SHARED DevsDeleteConnection.cpp)

TARGET_LINK_LIBRARIES(DevsDeleteConnection vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

INSTALL(TARGETS DevsDeleteConnection RUNTIME DESTINATION
  "${VLE_LIBRARY_DIRS}/simulator" LIBRARY DESTINATION
  "${VLE_LIBRARY_DIRS}/simulator")

ADD_LIBRARY(DevsTransform SHARED DevsTransform.cpp)

TARGET_LINK_LIBRARIES(DevsTransform vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

INSTALL(TARGETS DevsTransform RUNTIME DESTINATION
  "${VLE_LIBRARY_DIRS}/simulator" LIBRARY DESTINATION
  "${VLE_LIBRARY_DIRS}/simulator")

ADD_EXECUTABLE(test_time time.cpp)

TARGET_LINK_LIBRARIES(test_time vlelib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_TEST(devsttime test_time)
