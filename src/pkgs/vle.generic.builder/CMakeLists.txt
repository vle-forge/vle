function(DeclareSimulator targetname package name sources)
  include_directories(
    ${VLE_BINARY_DIR}/src
    ${VLE_SOURCE_DIR}/src
    ${VLEDEPS_INCLUDE_DIRS})

  link_directories(
    ${VLEDEPS_LIBRARY_DIRS})

  add_library(${targetname} MODULE ${sources})

  target_link_libraries(${targetname}
    vlelib ${VLE_LIBRARIES})

  set_target_properties(${targetname} PROPERTIES
    OUTPUT_NAME ${name}
    COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

  install(TARGETS ${targetname}
    RUNTIME DESTINATION lib/vle-${VLE_VERSION_SHORT}/pkgs/${package}/plugins/simulator
    LIBRARY DESTINATION lib/vle-${VLE_VERSION_SHORT}/pkgs/${package}/plugins/simulator
    ARCHIVE DESTINATION lib/vle-${VLE_VERSION_SHORT}/pkgs/${package}/plugins/simulator)
endfunction()

install(FILES Authors.txt Description.txt License.txt News.txt Readme.txt
  DESTINATION lib/vle-${VLE_VERSION_SHORT}/pkgs/vle.generic.builder)

DeclareSimulator(pkg-builder vle.generic.builder Builder src/Builder.cpp)
