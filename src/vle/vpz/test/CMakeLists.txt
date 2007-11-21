INCLUDE_DIRECTORIES(
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

ADD_LIBRARY(unittest SHARED tr1.cpp tr1.hpp)
TARGET_LINK_LIBRARIES(unittest
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES} 
  ${Boost_UNITTESTFRAMEWORK}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vledevs
  vlevpz)

INSTALL(TARGETS unittest
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/translator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/translator")

INSTALL(FILES unittest.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")

ADD_EXECUTABLE(test1 test1.cpp)
ADD_TEST(vpztest1 test1)
TARGET_LINK_LIBRARIES(test1
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNITTESTFRAMEWORK}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vledevs
  vlevpz)

ADD_EXECUTABLE(test2 test2.cpp)
ADD_TEST(vpztest2 test2)
TARGET_LINK_LIBRARIES(test2
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNITTESTFRAMEWORK}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vledevs
  vlevpz)

ADD_EXECUTABLE(test3 test3.cpp)
ADD_TEST(vpztest3 test3)
TARGET_LINK_LIBRARIES(test3
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNITTESTFRAMEWORK}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vledevs
  vlevpz)

ADD_EXECUTABLE(test4 test4.cpp)
ADD_TEST(vpztest4 test4)
TARGET_LINK_LIBRARIES(test4
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNITTESTFRAMEWORK}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vledevs
  vlevpz)

ADD_EXECUTABLE(test5 test5.cpp)
ADD_TEST(vpztest5 test5)
TARGET_LINK_LIBRARIES(test5
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${Boost_UNITTESTFRAMEWORK}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vledevs
  vlevpz)
