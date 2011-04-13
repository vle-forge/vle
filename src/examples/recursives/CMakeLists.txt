INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/recursives)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

DeclareModuleDynamics(RecursiveSimulation RecursiveSimulation.cpp)
DeclareModuleDynamics(SimpleSimulation SimpleSimulation.cpp)
DeclareModuleDynamics(TwoTimesModel TwoTimesModel.cpp)

INSTALL(FILES recursive.vpz twotimesmodel.vpz DESTINATION
  "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES RecursiveSimulation.cpp SimpleSimulation.cpp TwoTimesModel.cpp
  DESTINATION "${VLE_SHARE_DIRS}/examples/sources/recursives")

