function(DeclareSimulator targetname package name sources)
  include_directories(
    ${VLE_BINARY_DIR}/src
    ${VLE_SOURCE_DIR}/src
    ${VLEDEPS_INCLUDE_DIRS})

  link_directories(
    ${VLEDEPS_LIBRARY_DIRS})

  add_library(${targetname} MODULE ${sources})

  target_link_libraries(${targetname}
    vlelib ${VLEDEPS_LIBRARIES})

  set_target_properties(${targetname} PROPERTIES
    OUTPUT_NAME ${name}
    COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

  install(TARGETS ${targetname}
    RUNTIME DESTINATION lib/vle-${VLE_VERSION_SHORT}/pkgs/${package}/plugins/simulator
    LIBRARY DESTINATION lib/vle-${VLE_VERSION_SHORT}/pkgs/${package}/plugins/simulator
    ARCHIVE DESTINATION lib/vle-${VLE_VERSION_SHORT}/pkgs/${package}/plugins/simulator)
endfunction()

install(FILES Authors.txt Description.txt License.txt News.txt Readme.txt
  DESTINATION lib/vle-${VLE_VERSION_SHORT}/pkgs/vle.adaptative-qss)

DeclareSimulator(pkg-adder vle.adaptative-qss Adder src/Adder.cpp)
DeclareSimulator(pkg-generator vle.adaptative-qss Generator src/Generator.cpp)
DeclareSimulator(pkg-quantifier vle.adaptative-qss AdaptativeQuantifier src/AdaptativeQuantifier.cpp)
DeclareSimulator(pkg-integrator vle.adaptative-qss Integrator src/Integrator.cpp)
DeclareSimulator(pkg-mult vle.adaptative-qss Mult src/Mult.cpp)
DeclareSimulator(pkg-constant vle.adaptative-qss Constant src/Constant.cpp)
DeclareSimulator(pkg-plot vle.adaptative-qss Plot src/Plot.cpp)
