include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS})

link_directories(${VLEDEPS_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

if (WIN32)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gvle.o
    COMMAND ${CMAKE_RC_COMPILER}
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/gvle.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/gvle.o)
  add_executable(gvle WIN32 main.cpp ${CMAKE_CURRENT_BINARY_DIR}/gvle.o)

  set_target_properties(gvle PROPERTIES
    COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

else (WIN32)
  add_executable(gvle WIN32 main.cpp)

  set_target_properties(gvle PROPERTIES
    SOVERSION "${VLE_MAJOR}.${VLE_MINOR}"
    VERSION "${VLE_MAJOR}.${VLE_MINOR}"
    COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")
endif (WIN32)

target_link_libraries(gvle gvlelib vlelib ${VLEDEPS_LIBRARIES}
     ${Boost_LIBRARIES} ${OS_SPECIFIC_LIBRARIES})

install(TARGETS gvle DESTINATION bin)

install(FILES gvle.1 DESTINATION ${VLE_MANPAGE_PATH}
  RENAME "gvle-${VLE_VERSION_SHORT}.1")
