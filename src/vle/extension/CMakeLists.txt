INCLUDE_DIRECTORIES(
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR})

SET(EXTENSION_CPP CellDevs.cpp CellQSS.cpp CombinedQSS.cpp DESS.cpp
  DifferenceEquation.cpp DifferentialEquation.cpp DSDevs.cpp PetriNet.cpp
  QSS2.cpp QSS.cpp)

SET(EXTENSION_HPP CellDevs.hpp CellQSS.hpp CombinedQSS.hpp DESS.hpp
  DifferenceEquation.hpp DifferentialEquation.hpp DSDevs.hpp PetriNet.hpp
  QSS2.hpp QSS.hpp)

ADD_LIBRARY(vleextension SHARED ${EXTENSION_HPP};${EXTENSION_CPP})

SET_TARGET_PROPERTIES(vleextension PROPERTIES ${VLE_LIBRARY_PROPERTIES})

TARGET_LINK_LIBRARIES(vleextension
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
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
