INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/petrinet)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

DeclareModuleDynamics(PetrinetBeep PetrinetBeep.cpp)
DeclareModuleDynamics(PetrinetConflict PetrinetConflict.cpp)
DeclareModuleDynamics(PetrinetCounter PetrinetCounter.cpp)
DeclareModuleDynamics(PetrinetInOut PetrinetInOut.cpp)
DeclareModuleDynamics(PetrinetMeteo PetrinetMeteo.cpp)
DeclareModuleDynamics(PetrinetOrdinary PetrinetOrdinary.cpp)
DeclareModuleDynamics(PetrinetTrigger PetrinetTrigger.cpp)

INSTALL(FILES petrinet-and-timed.vpz petrinet-and.vpz petrinet-conflict.vpz
  petrinet-inout.vpz petrinet-meteo.vpz petrinet-mult.vpz petrinet-nand1.vpz
  petrinet-nand2.vpz petrinet-or-priority.vpz petrinet-or.vpz
  petrinet-simple.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES PetrinetBeep.cpp PetrinetConflict.cpp PetrinetCounter.cpp
  PetrinetInOut.cpp PetrinetMeteo.cpp PetrinetOrdinary.cpp PetrinetTrigger.cpp
  DESTINATION "${VLE_SHARE_DIRS}/examples/sources/petrinet")
