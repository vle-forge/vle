INCLUDE_DIRECTORIES(
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR})

SET(VALUE_HPP Boolean.hpp Double.hpp Integer.hpp Map.hpp Matrix.hpp Null.hpp
  Set.hpp String.hpp Table.hpp Tuple.hpp Value.hpp XML.hpp)

SET(VALUE_CPP Boolean.cpp Double.cpp Integer.cpp Map.cpp Matrix.cpp Null.cpp
  Set.cpp String.cpp Table.cpp Tuple.cpp Value.cpp XML.cpp)

ADD_LIBRARY(vlevalue SHARED ${VALUE_HPP};${VALUE_CPP})

TARGET_LINK_LIBRARIES(vlevalue
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vlegeometry
  vleutils
  vledata)

INSTALL(TARGETS vlevalue
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

INSTALL(FILES ${VALUE_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/value)

IF (HAVE_UNITTESTFRAMEWORK)
  ADD_SUBDIRECTORY(test)
ENDIF (HAVE_UNITTESTFRAMEWORK)
