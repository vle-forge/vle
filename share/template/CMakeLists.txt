install(DIRECTORY package
    DESTINATION ${CMAKE_INSTALL_DATADIR}/vle-${VLE_ABI}/template)

install(FILES unittest.vpz
    DESTINATION ${CMAKE_INSTALL_DATADIR}/vle-${VLE_ABI}/template)
