include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS})

link_directories(${VLEDEPS_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

if (WIN32)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vle.o
    COMMAND ${CMAKE_RC_COMPILER}
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/vle.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/vle.o)
  add_executable(vle main.cpp ${CMAKE_CURRENT_BINARY_DIR}/vle.o)

  set_target_properties(vle PROPERTIES
    COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

else (WIN32)
  add_executable(vle main.cpp)

  set_target_properties(vle PROPERTIES
     SOVERSION "${VLE_MAJOR}.${VLE_MINOR}"
     VERSION "${VLE_MAJOR}.${VLE_MINOR}"
     COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")
endif (WIN32)

target_link_libraries(vle vlelib ${CMAKE_THREAD_LIBS_INIT}
  ${VLEDEPS_LIBRARIES} ${OS_SPECIFIC_LIBRARIES})

install(TARGETS vle DESTINATION bin)

install(FILES vle.1 DESTINATION ${VLE_MANPAGE_PATH}
  RENAME "vle-${VLE_VERSION_SHORT}.1")
