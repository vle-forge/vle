INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR})

IF(WIN32)
  ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vfl.o
    COMMAND /usr/bin/i586-mingw32msvc-windres
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/vfl.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
  SET(OS_SPECIFIC_PATH_IMPL ${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
ELSE(WIN32)
  SET(OS_SPECIFIC_PATH_IMPL)
ENDIF(WIN32)

SET(EXTENSION_CPP CellDevs.cpp CellQSS.cpp DESS.cpp
  DifferenceEquation.cpp DifferentialEquation.cpp DSDevs.cpp PetriNet.cpp
  QSS2.cpp QSS.cpp FSA.cpp Moore.cpp Mealy.cpp Statechart.cpp FDDevs.cpp
  ${OS_SPECIFIC_PATH_IMPL})

SET(EXTENSION_HPP CellDevs.hpp CellQSS.hpp DESS.hpp DllDefines.hpp
  DifferenceEquation.hpp DifferentialEquation.hpp DSDevs.hpp PetriNet.hpp
  QSS2.hpp QSS.hpp FSA.hpp Moore.hpp Mealy.hpp Statechart.hpp FDDevs.hpp)

ADD_LIBRARY(vleextension SHARED ${EXTENSION_HPP};${EXTENSION_CPP})

SET_TARGET_PROPERTIES(vleextension PROPERTIES ${VLE_LIBRARY_PROPERTIES})
SET_TARGET_PROPERTIES(vleextension PROPERTIES OUTPUT_NAME
  "vleextension-${VLE_VERSION_SHORT}")

TARGET_LINK_LIBRARIES(vleextension
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${OS_SPECIFIC_LIBRARIES}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vleoov
  vlevpz
  vledevs)

INSTALL(TARGETS vleextension
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

INSTALL(FILES ${EXTENSION_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/extension)
