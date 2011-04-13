INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/examples/lifegame)

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

DeclareModuleDynamics(LifegameCell Cell.cpp)
DeclareModuleDynamics(LifegameExecutive Executive.cpp)

INSTALL(FILES lifegame_txt.vpz lifegame.vpz lifegamewithclass_txt.vpz
  lifegamewithclass.vpz DESTINATION "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES Cell.cpp Executive.cpp DESTINATION
  "${VLE_SHARE_DIRS}/examples/sources/lifegame")
