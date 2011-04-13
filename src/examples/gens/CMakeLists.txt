INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${VLE_SOURCE_DIR}/src/examples/gens)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

DeclareModuleDynamics(GensBeep Beep.cpp)
DeclareModuleDynamics(GensCounter Counter.cpp)
DeclareModuleDynamics(GensExecutive GenExecutive.cpp)
DeclareModuleDynamics(GensExecutiveClass GenExecutiveClass.cpp)
DeclareModuleDynamics(GensExecutiveGraph GenExecutiveGraph.cpp)

INSTALL(FILES gensdelete.vpz gensgraph.vpz gensnetview.vpz gens.vpz
  genswithclass.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES Beep.cpp Counter.cpp GenExecutiveClass.cpp GenExecutive.cpp
  GenExecutiveGraph.cpp DESTINATION "${VLE_SHARE_DIRS}/examples/sources/gens")
