include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS} ${MPI_INCLUDE_PATH})

link_directories(${VLEDEPS_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

if (WIN32)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vle.o
    COMMAND ${CMAKE_RC_COMPILER}
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/vle.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/vle.o)
  add_executable(mvle mvle.cpp ${CMAKE_CURRENT_BINARY_DIR}/vle.o)
else ()
  add_executable(mvle mvle.cpp)
  set_target_properties(mvle PROPERTIES SOVERSION
    "${VLE_MAJOR}.${VLE_MINOR}" VERSION "${VLE_MAJOR}.${VLE_MINOR}")
endif ()

if (WIN32)
  set_target_properties(mvle PROPERTIES LINK_FLAGS "-Wl,-static")
endif ()
target_link_libraries(mvle ${VLEDEPS_LIBRARIES}
  ${OS_SPECIFIC_LIBRARIES} ${Boost_LIBRARIES} ${MPI_LIBRARY}
  ${MPI_EXTRA_LIBRARY} vlelib)

install(TARGETS mvle DESTINATION bin)

install(FILES mvle.1 DESTINATION ${VLE_MANPAGE_PATH} RENAME
  "mvle-${VLE_VERSION_SHORT}.1")

if (Boost_MPI_FOUND AND VLE_HAVE_CVLE)
  add_executable(cvle cvle.cpp
       ${VLE_SOURCE_DIR}/src/vle/vpz/SaxParser.cpp
       ${VLE_SOURCE_DIR}/src/vle/vpz/SaxStackValue.cpp
       ${VLE_SOURCE_DIR}/src/vle/vpz/SaxStackVpz.cpp)
  set_target_properties(cvle PROPERTIES SOVERSION
    "${VLE_MAJOR}.${VLE_MINOR}" VERSION "${VLE_MAJOR}.${VLE_MINOR}")
  target_link_libraries(cvle ${VLEDEPS_LIBRARIES}
    ${OS_SPECIFIC_LIBRARIES} ${Boost_LIBRARIES} ${MPI_LIBRARY}
    ${MPI_EXTRA_LIBRARY} vlelib)

  install(TARGETS cvle DESTINATION bin)
  install(FILES cvle.1 DESTINATION ${VLE_MANPAGE_PATH} RENAME
    "mvle-${VLE_VERSION_SHORT}.1")
endif()

