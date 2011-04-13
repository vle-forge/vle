INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/qss)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})


DeclareModuleDynamics(QssDifferenceEquationAdaptor DifferenceEquationAdaptor.cpp)
DeclareModuleDynamics(QssLadyBird2 ladybird2.cpp)
DeclareModuleDynamics(QssLadyBird ladybird.cpp)
DeclareModuleDynamics(QssLadyBirdPlantLouse ladybirdplantlouse.cpp)
DeclareModuleDynamics(QssPlantLouse2 plantlouse2.cpp)
DeclareModuleDynamics(QssPlantLouse plantlouse.cpp)
DeclareModuleDynamics(QssSmartGardener2 smartgardener2.cpp)
DeclareModuleDynamics(QssSmartGardener smartgardener.cpp)

INSTALL(FILES qss_exp1_test.vpz qss_exp1_txt.vpz qss_exp1.vpz qss_exp2_txt.vpz
  qss_exp2.vpz qss_exp3_test.vpz qss_exp3_txt.vpz qss_exp3.vpz qss_exp4_txt.vpz
  qss_exp4.vpz qss_exp5.vpz qss_exp6.vpz qss_exp7.vpz DESTINATION
  "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES DifferenceEquationAdaptor.cpp ladybird2.cpp ladybird.cpp
  ladybirdplantlouse.cpp plantlouse2.cpp plantlouse.cpp smartgardener2.cpp
  smartgardener.cpp DESTINATION "${VLE_SHARE_DIRS}/examples/sources/qss")
