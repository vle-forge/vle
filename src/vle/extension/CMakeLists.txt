INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR})

SET(EXTENSION_CPP CellDevs.cpp CellQSS.cpp DESS.cpp
  DifferenceEquation.cpp DifferentialEquation.cpp DSDevs.cpp PetriNet.cpp
  QSS2.cpp QSS.cpp)

SET(EXTENSION_HPP CellDevs.hpp CellQSS.hpp DESS.hpp DllDefines.hpp
  DifferenceEquation.hpp DifferentialEquation.hpp DSDevs.hpp PetriNet.hpp
  QSS2.hpp QSS.hpp FSA.hpp FSA.cpp Moore.hpp Moore.cpp Mealy.hpp
  Mealy.cpp Statechart.hpp Statechart.cpp FDDevs.hpp)

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
  vledata
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
