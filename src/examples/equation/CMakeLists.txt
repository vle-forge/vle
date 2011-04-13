INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/equation)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

DeclareModuleDynamics(EquationA1 A1.cpp A1.hpp)
DeclareModuleDynamics(EquationA2 A2.cpp A2.hpp)
DeclareModuleDynamics(EquationAsynchronousE0 asynchronousE0.cpp)
DeclareModuleDynamics(EquationAsynchronousE1 asynchronousE1.cpp)
DeclareModuleDynamics(EquationAsynchronousE2 asynchronousE2.cpp)
DeclareModuleDynamics(EquationAsynchronousE3 asynchronousE3.cpp)
DeclareModuleDynamics(EquationAsynchronousFSA asynchronousFSA.cpp)
DeclareModuleDynamics(EquationB1 B1.cpp B1.hpp)
DeclareModuleDynamics(EquationB2 B2.cpp B2.hpp)
DeclareModuleDynamics(EquationB3 B3.cpp)
DeclareModuleDynamics(EquationC1 C1.cpp C1.hpp)
DeclareModuleDynamics(EquationC2 C2.cpp C2.hpp)
DeclareModuleDynamics(EquationC3 C3.cpp)
DeclareModuleDynamics(EquationD1 D1.cpp D1.hpp)
DeclareModuleDynamics(EquationD2 D2.cpp D2.hpp)
DeclareModuleDynamics(EquationD3 D3.cpp)
DeclareModuleDynamics(EquationE1 E1.cpp E1.hpp)
DeclareModuleDynamics(EquationE2 E2.cpp E2.hpp)
DeclareModuleDynamics(EquationEarlyPerturbE0 earlyPerturbE0.cpp)
DeclareModuleDynamics(EquationEarlyPerturbE1 earlyPerturbE1.cpp)
DeclareModuleDynamics(EquationEarlyPerturbE2 earlyPerturbE2.cpp)
DeclareModuleDynamics(EquationEarlyPerturbFSA earlyPerturbFSA.cpp)
DeclareModuleDynamics(EquationF1 F1.cpp F1.hpp)
DeclareModuleDynamics(EquationG1 G1.cpp G1.hpp)
DeclareModuleDynamics(EquationPerturb1 Perturb1.cpp)
DeclareModuleDynamics(EquationPerturb2 Perturb2.cpp)
DeclareModuleDynamics(EquationPerturb3 Perturb3.cpp)
DeclareModuleDynamics(EquationPerturb4 Perturb4.cpp)
DeclareModuleDynamics(EquationPerturb5 Perturb5.cpp)
DeclareModuleDynamics(EquationPerturb6 Perturb6.cpp)
DeclareModuleDynamics(EquationPerturb7 Perturb7.cpp)
DeclareModuleDynamics(EquationPerturb8 Perturb8.cpp)
DeclareModuleDynamics(EquationPerturb9 Perturb9.cpp)
DeclareModuleDynamics(EquationPerturb10 Perturb10.cpp)
DeclareModuleDynamics(EquationSum sum.cpp)

INSTALL(FILES equation1.vpz equation2.vpz equation3.vpz equation4.vpz
  equation5.vpz equation6.vpz equation7.vpz equation8.vpz equation9.vpz
  equation10.vpz equation11.vpz equation12.vpz equation13.vpz equation14.vpz
  equation15.vpz asynchronous.vpz earlyPerturb.vpz
  DESTINATION "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES A1.cpp A1.hpp A2.cpp A2.hpp asynchronousE0.cpp asynchronousE1.cpp
  asynchronousE2.cpp asynchronousE3.cpp asynchronousFSA.cpp B1.cpp B1.hpp B2.cpp
  B2.hpp B3.cpp C1.cpp C1.hpp C2.cpp C2.hpp C3.cpp D1.cpp D1.hpp D2.cpp D2.hpp
  D3.cpp E1.cpp E1.hpp E2.cpp E2.hpp earlyPerturbE0.cpp earlyPerturbE1.cpp
  earlyPerturbE2.cpp earlyPerturbFSA.cpp F1.cpp F1.hpp G1.cpp G1.hpp
  Perturb10.cpp Perturb1.cpp Perturb2.cpp Perturb3.cpp Perturb4.cpp Perturb5.cpp
  Perturb6.cpp Perturb7.cpp Perturb8.cpp Perturb9.cpp sum.cpp
  DESTINATION "${VLE_SHARE_DIRS}/examples/sources/equation")
