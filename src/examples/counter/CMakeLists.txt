INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/counter)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

SET(SOURCES1_CPP Counter.cpp Counter.hpp)
  
ADD_LIBRARY(counter SHARED ${SOURCES1_CPP})

TARGET_LINK_LIBRARIES(counter
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vleutils
  vlevalue
  vlegraph
  vlevpz
  vleoov
  vledevs
  vlemanager
  vletranslator)

SET(SOURCES2_CPP Generator.hpp GeneratorWrapper.cpp GeneratorWrapper.hpp
  LogNormalGenerator.cpp LogNormalGenerator.hpp NormalGenerator.cpp
  NormalGenerator.hpp RandomGenerator.cpp RandomGenerator.hpp
  RegularGenerator.hpp UniformGenerator.cpp UniformGenerator.hpp)

ADD_LIBRARY(generator SHARED ${SOURCES2_CPP})

TARGET_LINK_LIBRARIES(generator
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vleutils
  vlevalue
  vlegraph
  vlevpz
  vleoov
  vledevs
  vlemanager
  vletranslator)

INSTALL(TARGETS generator
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

INSTALL(TARGETS counter
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")

INSTALL(FILES counter.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES ${SOURCES1_CPP} ${SOURCES2_CPP} DESTINATION
  "${VLE_SHARE_DIRS}/examples/sources/counter")
