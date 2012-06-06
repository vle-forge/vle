set(DOXYGEN_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(DOXYGEN_OUTPUT_DIR "${VLE_BINARY_DIR}/doxygen")
IF (WIN32)
  find_package(HTMLHelp)
  if (HTML_HELP_COMPILER)
    set(DOXYGEN_BUILD_CHM "YES")
  endif (HTML_HELP_COMPILER)
endif (WIN32)

configure_file(doxygen.conf.in "${VLE_BINARY_DIR}/doxygen.conf")
file(MAKE_DIRECTORY "${VLE_BINARY_DIR}/doxygen/html")

add_custom_command(
  OUTPUT "${VLE_BINARY_DIR}/doxygen/html/index.html"
  DEPENDS "${VLE_BINARY_DIR}/doxygen.conf"
  COMMAND "${DOXYGEN}"
  ARGS "${VLE_BINARY_DIR}/doxygen.conf")

add_custom_target(doc ALL DEPENDS "${VLE_BINARY_DIR}/doxygen.conf"
  "${VLE_BINARY_DIR}/doxygen/html/index.html" VERBATIM)


if (WIN32)
  if (HTML_HELP_COMPILER)
    set(TMP "${VLE_BINARY_DIR}\\doxygen\\html\\index.hhp")
    string(REGEX REPLACE "[/]" "\\\\" HHP_FILE ${TMP} )
    add_custom_target(winhelp ALL "${HTML_HELP_COMPILER}" "${HHP_FILE}")
    add_dependencies (winhelp doc)

    if (EXISTS "${VLE_BINARY_DIR}/doxygen/html/vle.chm")
      install(FILES "${VLE_BINARY_DIR}/doxygen/html/vle.chm"
	    DESTINATION "${VLE_SHARE_DIRS}/doc")
    endif (EXISTS "${VLE_BINARY_DIR}/doxygen/html/vle.chm")
  endif(HTML_HELP_COMPILER)
else (WIN32)
  install(DIRECTORY "${VLE_BINARY_DIR}/doxygen/html"
    DESTINATION "${VLE_SHARE_DIRS}/doc")
endif (WIN32)

