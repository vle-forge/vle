INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/random)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

ADD_LIBRARY(generator SHARED Generator.hpp GeneratorWrapper.cpp
  GeneratorWrapper.hpp LogNormalGenerator.cpp LogNormalGenerator.hpp
  NormalGenerator.cpp NormalGenerator.hpp RandomGenerator.cpp
  RandomGenerator.hpp RegularGenerator.hpp UniformGenerator.cpp
  UniformGenerator.hpp)

TARGET_LINK_LIBRARIES(generator
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vlegeometry
  vleutils
  vledata
  vlevalue
  vlegraph
  vlevpz
  vleoov
  vledevs
  vleextension
  vlemanager
  vletranslator)

INSTALL(TARGETS generator
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")
