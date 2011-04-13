INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/fsa)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})


DeclareModuleDynamics(FDDevs1 FDDevs1.cpp)
DeclareModuleDynamics(FDDevs2 FDDevs2.cpp)
DeclareModuleDynamics(Generator Generator.cpp)
DeclareModuleDynamics(LinearEqLin LinearEqLin.cpp)
DeclareModuleDynamics(LinearGenLin LinearGenLin.cpp)
DeclareModuleDynamics(Mealy1 Mealy1.cpp)
DeclareModuleDynamics(MealyCounter1 MealyCounter1.cpp)
DeclareModuleDynamics(Meteo Meteo.cpp)
DeclareModuleDynamics(Moore1 Moore1.cpp)
DeclareModuleDynamics(MooreCounter2 MooreCounter2.cpp)
DeclareModuleDynamics(Stage Stage.cpp Stage.hpp)
DeclareModuleDynamics(Statechart1 Statechart1.cpp)
DeclareModuleDynamics(Statechart2 Statechart2.cpp)
DeclareModuleDynamics(Statechart3 Statechart3.cpp)
DeclareModuleDynamics(Statechart4 Statechart4.cpp)
DeclareModuleDynamics(Statechart5 Statechart5.cpp)
DeclareModuleDynamics(Statechart6 Statechart6.cpp)
DeclareModuleDynamics(Statechart7 Statechart7.cpp)
DeclareModuleDynamics(Statechart8 Statechart8.cpp)
DeclareModuleDynamics(Statechart9 Statechart9.cpp)
DeclareModuleDynamics(StatechartListener StatechartListener.cpp)
DeclareModuleDynamics(StatechartMultipleSend StatechartMultipleSend.cpp)
DeclareModuleDynamics(StatechartThreshold StatechartThreshold.cpp)

INSTALL(FILES fddevs.vpz mealy2.vpz mealy.vpz moore2.vpz moore.vpz stage.vpz
  statechart10.vpz statechart11.vpz statechart2.vpz statechart3.vpz
  statechart4.vpz statechart5.vpz statechart6.vpz statechart7.vpz
  statechart8.vpz statechart9.vpz statechartMultipleSend.vpz statechart.vpz
  DESTINATION "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES FDDevs1.cpp FDDevs2.cpp Generator.cpp LinearEqLin.cpp
  LinearGenLin.cpp Mealy1.cpp MealyCounter1.cpp Meteo.cpp Moore1.cpp
  MooreCounter2.cpp Stage.cpp Stage.hpp Statechart1.cpp Statechart2.cpp
  Statechart3.cpp Statechart4.cpp Statechart5.cpp Statechart6.cpp
  Statechart7.cpp Statechart8.cpp Statechart9.cpp StatechartListener.cpp
  StatechartMultipleSend.cpp StatechartThreshold.cpp
  DESTINATION "${VLE_SHARE_DIRS}/examples/sources/fsa")
