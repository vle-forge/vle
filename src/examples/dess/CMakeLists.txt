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

DeclareModuleDynamics(DessCounter Counter.cpp Counter.hpp)
DeclareModuleDynamics(DessE E.cpp E.hpp)
DeclareModuleDynamics(DessI2 I2.cpp I2.hpp)
DeclareModuleDynamics(DessI3 I3.cpp I3.hpp)
DeclareModuleDynamics(DessI I.cpp I.hpp)
DeclareModuleDynamics(DessR2 R2.cpp R2.hpp)
DeclareModuleDynamics(DessR3 R3.cpp R3.hpp)
DeclareModuleDynamics(DessR R.cpp R.hpp)
DeclareModuleDynamics(DessS2 S2.cpp S2.hpp)
DeclareModuleDynamics(DessS3 S3.cpp S3.hpp)
DeclareModuleDynamics(DessS S.cpp S.hpp)
DeclareModuleDynamics(DessSeir seir.cpp seir.hpp)
DeclareModuleDynamics(DessSir sir.cpp sir.hpp)

INSTALL(FILES seir2.vpz seir.vpz sir2.vpz sir3.vpz sir4.vpz sir.vpz DESTINATION
  "${VLE_SHARE_DIRS}/examples")

INSTALL(FILES Counter.cpp Counter.hpp E.cpp E.hpp I2.cpp I2.hpp I3.cpp I3.hpp
  I.cpp I.hpp R2.cpp R2.hpp R3.cpp R3.hpp R.cpp R.hpp S2.cpp S2.hpp S3.cpp
  S3.hpp S.cpp seir.cpp seir.hpp S.hpp sir.cpp sir.hpp DESTINATION
  "${VLE_SHARE_DIRS}/examples/sources/dess")
