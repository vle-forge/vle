include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${EXPAT_INCLUDE_DIRS} ${MPI_INCLUDE_PATH})

link_directories(${Boost_LIBRARY_DIRS})

if (VLE_HAVE_MVLE)
  add_executable(mvle mvle.cpp)
  set_target_properties(mvle PROPERTIES SOVERSION
    "${VLE_MAJOR}.${VLE_MINOR}" VERSION "${VLE_MAJOR}.${VLE_MINOR}")
  target_link_libraries(mvle ${EXPAT_LIBRARIES}
    ${OS_SPECIFIC_LIBRARIES} ${Boost_LIBRARIES} ${MPI_LIBRARY}
    ${MPI_EXTRA_LIBRARY} vlelib)

  install(TARGETS mvle DESTINATION bin)
endif()

if (VLE_HAVE_CVLE)
  add_executable(cvle cvle.cpp
       ${VLE_SOURCE_DIR}/src/vle/vpz/SaxParser.cpp
       ${VLE_SOURCE_DIR}/src/vle/vpz/SaxStackValue.cpp
       ${VLE_SOURCE_DIR}/src/vle/vpz/SaxStackVpz.cpp)
  set_target_properties(cvle PROPERTIES SOVERSION
    "${VLE_MAJOR}.${VLE_MINOR}" VERSION "${VLE_MAJOR}.${VLE_MINOR}")
  target_link_libraries(cvle ${EXPAT_LIBRARIES}
    ${OS_SPECIFIC_LIBRARIES} ${Boost_LIBRARIES} ${MPI_LIBRARY}
    ${MPI_EXTRA_LIBRARY} vlelib)

  install(TARGETS cvle DESTINATION bin)
endif()
