if (${ASCIIDOCTOR_EXECUTABLE})
  set(VLE_DOCDIR ${CMAKE_CURRENT_SOURCE_DIR})
  set(VLE_STYLESHEET ${VLE_DOCDIR}/stylesheet.css)
  set(VLE_TITLE ${E_NAME})
  set(VLE_A2M ${ASCIIDOCTOR_EXECUTABLE} -b manpage -amanmanual='${VLE_TITLE}')
  set(VLE_A2H ${ASCIIDOCTOR_EXECUTABLE} -d manpage -b html5 -a stylesheeet=${VLE_STYLESHEET} -aversion-label=${E_NAME} -arevnumber=${E_NAME})

  macro (add_man NAME SECT)
    add_custom_command(
      OUTPUT ${NAME}.${SECT}
      COMMAND ${VLE_A2M} -o ${NAME}.${SECT} ${VLE_DOCDIR}/${NAME}.adoc
      MAIN_DEPENDENCY ${VLE_DOCDIR}/${NAME}.adoc)

    add_custom_command(
      OUTPUT ${NAME}.${SECT}.html
      COMMAND ${VLE_A2H} -o ${NAME}.${SECT}.html ${VLE_DOCDIR}/${NAME}.adoc
      DEPENDS ${VLE_STYLESHEET}
      MAIN_DEPENDENCY ${VLE_DOCDIR}/${NAME}.adoc)

    set(VLE_MANS ${VLE_MANS} ${NAME}.${SECT})
    set(VLE_HTMLS ${VLE_HTMLS} ${NAME}.${SECT}.html)

    install(
      FILES ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.${SECT}.html
      DESTINATION ${CMAKE_INSTALL_DOCDIR})

    install(
      FILES ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.${SECT}
      DESTINATION ${CMAKE_INSTALL_MANDIR}/man${SECT})
  endmacro (add_man)

  add_man(vle 1)
  add_man(cvle 1)
  add_man(mvle 1)

  add_custom_target (man ALL DEPENDS ${VLE_MANS})
  add_custom_target (html ALL DEPENDS ${VLE_HTMLS})
endif ()
install(FILES README README_win32.adoc DESTINATION ${CMAKE_INSTALL_DOCDIR})
