include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS})

link_directories(${VLEDEPS_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

if (WIN32)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vle.o
    COMMAND ${CMAKE_RC_COMPILER}
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/vle.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/vle.o)
  add_executable(vle main.cpp OptionGroup.cpp OptionGroup.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/vle.o)
else ()
  add_executable(vle main.cpp OptionGroup.cpp OptionGroup.hpp)
  set_target_properties(vle PROPERTIES SOVERSION
    "${VLE_MAJOR}.${VLE_MINOR}" VERSION "${VLE_MAJOR}.${VLE_MINOR}")
endif ()

if (BUILD_SHARED_LIBS)
  target_link_libraries(vle ${VLEDEPS_LIBRARIES}
    ${OS_SPECIFIC_LIBRARIES} ${Boost_LIBRARIES} vlelib)
else ()
  if (WIN32)
    set_target_properties(vle PROPERTIES LINK_FLAGS "-Wl,-static")
  endif ()
  target_link_libraries(vle vlelib-static)
endif ()

install(TARGETS vle DESTINATION bin)

install(FILES vle.1 DESTINATION ${VLE_MANPAGE_PATH}
  RENAME "vle-${VLE_VERSION_SHORT}.1")
