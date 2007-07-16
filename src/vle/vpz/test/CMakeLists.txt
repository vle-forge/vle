INCLUDE_DIRECTORIES(
	${VLE_SOURCE_DIR}/src
	${GLIBMM_INCLUDE_DIRS}
        ${LIBXMLPP_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS})

LINK_DIRECTORIES(
	${GLIBMM_LIBRARY_DIRS}
	${LIBXMLPP_LIBRARY_DIR}
        ${Boost_LIBRARY_DIRS})


ADD_LIBRARY(vletesttr1 SHARED tr1.cpp tr1.hpp)
TARGET_LINK_LIBRARIES(vletesttr1
        ${GLIBMM_LIBRARIES}
        ${LIBXMLPP_LIBRARIES}
        ${GTHREAD_LIBRARIES} 
	vlegeometry
	vleutils
	vledata
	vlevalue
        vlegraph
        vledevs
        vlevpz)
INSTALL(TARGETS vletesttr1 DESTINATION "${VLE_LIBRARY_DIRS}/translator")

ADD_EXECUTABLE(test1 test1.cpp)
ADD_TEST(vpztest1 test1)
TARGET_LINK_LIBRARIES(test1
	${GLIBMM_LIBRARIES}
	${LIBXMLPP_LIBRARIES}
	${GTHREAD_LIBRARIES}
        boost_unit_test_framework
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
        boost_unit_test_framework
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
        boost_unit_test_framework
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
        boost_unit_test_framework
	vlegeometry
	vleutils
	vledata
	vlevalue
	vlegraph
        vledevs
        vlevpz)
