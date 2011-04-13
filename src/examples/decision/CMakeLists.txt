INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})


DeclareModuleDynamics(DecisionCounter DecisionCounter.cpp)
DeclareModuleDynamics(DecisionRetarder DecisionRetarder.cpp)
DeclareModuleDynamics(DecisionStart DecisionStart.cpp)
DeclareModuleDynamics(OnlyAgent OnlyAgent.cpp)
DeclareModuleDynamics(OnlyAgentPrecedenceConstraint OnlyAgentPrecedenceConstraint.cpp)
DeclareModuleDynamics(OnlyAgentWakeUp OnlyAgentWakeUp.cpp)
DeclareModuleDynamics(SimpleAgent SimpleAgent.cpp)

INSTALL(FILES agentonlyc.vpz agentonly.vpz agentonlywakeup.vpz DESTINATION
  "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES DecisionCounter.cpp DecisionRetarder.cpp DecisionStart.cpp
  OnlyAgent.cpp OnlyAgentPrecedenceConstraint.cpp OnlyAgentWakeUp.cpp
  SimpleAgent.cpp DESTINATION "${VLE_SHARE_DIRS}/examples/sources/decision")
