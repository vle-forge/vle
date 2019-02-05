add_subdirectory(doc)
add_subdirectory(pixmaps)
add_subdirectory(dtd)
add_subdirectory(template)
add_subdirectory(cmake)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/gvle.rc.in
  ${CMAKE_CURRENT_BINARY_DIR}/gvle.rc @ONLY)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vfl.rc.in
  ${CMAKE_CURRENT_BINARY_DIR}/vfl.rc @ONLY)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vle.rc.in
  ${CMAKE_CURRENT_BINARY_DIR}/vle.rc @ONLY)

install(FILES convert-vpz11-vpz12.py
    DESTINATION ${CMAKE_INSTALL_DATADIR}/vle-${VLE_ABI}/scripts)

if(WIN32)
  install(FILES vle.iss qt.conf
    DESTINATION ${CMAKE_INSTALL_DATADIR}/vle-${VLE_ABI})
endif()
