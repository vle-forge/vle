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
    COMMAND ${CMAKE_RC_COMPILER}
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/vfl.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
  SET(OS_SPECIFIC_PATH_IMPL ${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
ELSE(WIN32)
  SET(OS_SPECIFIC_PATH_IMPL)
ENDIF(WIN32)

SET(EXTENSION_CPP CellDevs.cpp CellQSS.cpp DSDevs.cpp PetriNet.cpp
  ${OS_SPECIFIC_PATH_IMPL})

SET(EXTENSION_HPP CellDevs.hpp CellQSS.hpp DllDefines.hpp
  DifferentialEquation.hpp DSDevs.hpp FSA.hpp PetriNet.hpp)

SET(EXT_FSA_CPP fsa/FSA.cpp fsa/Moore.cpp fsa/Mealy.cpp fsa/FDDevs.cpp
  fsa/Statechart.cpp)

SET(EXT_FSA_HPP fsa/FSA.hpp fsa/Moore.hpp fsa/Mealy.hpp fsa/FDDevs.hpp
  fsa/Statechart.hpp fsa/StatechartDbg.hpp)

SET(EXT_DIFFERENTIAL_EQUATION_CPP differential-equation/DifferentialEquation.cpp
  differential-equation/DESS.cpp differential-equation/QSS.cpp
  differential-equation/QSS2.cpp)

SET(EXT_DIFFERENTIAL_EQUATION_HPP differential-equation/DifferentialEquation.hpp
  differential-equation/DESS.hpp differential-equation/QSS.hpp
  differential-equation/QSS2.hpp)

ADD_LIBRARY(vleextension SHARED ${EXTENSION_HPP};${EXTENSION_CPP};
  ${EXT_FSA_HPP};${EXT_FSA_CPP};
  ${EXT_DIFFERENTIAL_EQUATION_HPP};${EXT_DIFFERENTIAL_EQUATION_CPP})

SET_TARGET_PROPERTIES(vleextension PROPERTIES ${VLE_LIBRARY_PROPERTIES})
SET_TARGET_PROPERTIES(vleextension PROPERTIES OUTPUT_NAME
  "vleextension-${VLE_VERSION_SHORT}")

TARGET_LINK_LIBRARIES(vleextension
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${OS_SPECIFIC_LIBRARIES}
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

INSTALL(FILES ${EXTENSION_HPP} DESTINATION
  ${VLE_INCLUDE_DIRS}/extension)
INSTALL(FILES ${EXT_FSA_HPP} DESTINATION
  ${VLE_INCLUDE_DIRS}/extension/fsa)
INSTALL(FILES ${EXT_DIFFERENTIAL_EQUATION_HPP} DESTINATION
  ${VLE_INCLUDE_DIRS}/extension/differential-equation)
