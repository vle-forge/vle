INCLUDE_DIRECTORIES(
	${VLE_SOURCE_DIR}/src
	${GLIBMM_INCLUDE_DIRS}
        ${LIBXMLPP_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS})

LINK_DIRECTORIES(
	${GLIBMM_LIBRARY_DIRS}
	${LIBXMLPP_LIBRARY_DIR}
        ${Boost_LIBRARY_DIRS})


ADD_EXECUTABLE(test1 test1.cpp)
ADD_TEST(devstest1 test1)
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
