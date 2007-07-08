INCLUDE_DIRECTORIES(
	${VLE_SOURCE_DIR}/src
	${GLIBMM_INCLUDE_DIRS}
	${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
	${GLIBMM_LIBRARY_DIRS}
	${LIBXMLPP_LIBRARY_DIR})

SET(VALUE_HPP
    Boolean.hpp Double.hpp Integer.hpp Map.hpp Set.hpp String.hpp Table.hpp
    Tuple.hpp Value.hpp XML.hpp)

SET(VALUE_CPP
    Boolean.cpp Double.cpp Integer.cpp Map.cpp Set.cpp String.cpp Table.cpp
    Tuple.cpp Value.cpp XML.cpp)

ADD_LIBRARY(vlevalue SHARED ${VALUE_HPP};${VALUE_CPP})

INSTALL(TARGETS vlevalue DESTINATION ${VLE_LIBRARY_DIR})
INSTALL(FILES ${VALUE_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/value)
