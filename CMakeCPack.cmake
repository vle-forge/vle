include(InstallRequiredSystemLibraries)

#
# Common configuration
#

set(CPACK_PACKAGE_NAME "VLE")
set(CPACK_PACKAGE_VENDOR "VLE Development Team")
set(CPACK_PACKAGE_VERSION_MAJOR "${VLE_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VLE_MINOR}")
if ("${VLE_EXTRA}" STREQUAL "")
  set(CPACK_PACKAGE_VERSION_PATCH "${VLE_PATCH}")
else ()
  set(CPACK_PACKAGE_VERSION_PATCH "${VLE_PATCH}-${VLE_EXTRA}")
endif ()
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "VLE - Virtual Laboratory Environment")
if (NOT CMAKE_SYSTEM_PROCESSOR)
  if (NOT CMAKE_HOST_SYSTEM_PROCESSOR)
    set(CPACK_PACKAGE_FILE_NAME "${VLE_NAME_COMPLETE}-${CMAKE_SYSTEM_NAME}")
  else ()
    set(CPACK_PACKAGE_FILE_NAME "${VLE_NAME_COMPLETE}-${CMAKE_HOST_SYSTEM_PROCESSOR}")
  endif ()
else ()
  set(CPACK_PACKAGE_FILE_NAME "${VLE_NAME_COMPLETE}-${CMAKE_SYSTEM_PROCESSOR}")
endif ()
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${VLE_NAME_COMPLETE})
set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/share/pixmaps\\\\logo.bmp")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_EXECUTABLES "vle" "VLE" "gvle" "GVLE")
set(CPACK_STRIP_FILES "1")

#
# CPack source configuration
#
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${VLE_NAME_COMPLETE})
set(CPACK_SOURCE_IGNORE_FILES "\\\\.swp$;/\\\\.gitignore;/build/;/\\\\.git/")

#
# CPack DEB configuration
#

set(CPACK_DEBIAN_PACKAGE_NAME ${VLE_NAME}-${VLE_MAJOR}.${VLE_MINOR})
set(CPACK_DEBIAN_PACKAGE_VERSION ${VLE_COMPLETE})
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
set(CPACK_DEBIAN_PACKAGE_BUILDS_DEPENDS "")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Gauthier Quesnel <quesnel@users.sourceforge.net>")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "VLE, a framework for multi-modeling, simulation and analysis of complex dynamical systems.")
set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")

#
# CPack RPM configuration
#

set(CPACK_RPM_PACKAGE_SUMMARY "VLE, a framework for multi-modeling, simulation and analysis of complex dynamical systems.")
set(CPACK_RPM_PACKAGE_NAME ${VLE_NAME}-${VLE_MAJOR}.${VLE_MINOR})
set(CPACK_RPM_PACKAGE_VERSION ${VLE_MAJOR}.${VLE_MINOR}.${VLE_PATCH})
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_VENDOR "Gauthier Quesnel <quesnel@users.sourceforge.net>")
set(CPACK_RPM_PACKAGE_LICENSE "GNU General Public License (GPL)")
set(CPACK_RPM_PACKAGE_DESCRIPTION "VLE, a framework for multi-modeling, simulation and analysis of complex dynamical systems.")

#
# CPack NSIS configuration
#

set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} ${VLE_VERSION_SHORT}.0")
set(CPACK_CREATE_DESKTOP_LINKS gvle)

#set(CMAKE_MODULE_PATH "share")
#set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/share)

#set(QT_INSTALL_PATH CACHE PATH "Qt install path")
#set(VLE_MINGW_PATH CACHE PATH "Mingw Boost directory")
#set(VLEDEPS_PATH CACHE PATH "vle deps directory")
#set(VLE_BOOST_INCLUDE_PATH CACHE PATH "Boost include path")
#set(VLE_BOOST_LIBRARIES_PATH CACHE PATH "Boost libraries path")
#set(VLE_CMAKE_PATH CACHE PATH "CMake directory")
#set(Boost_INCLUDE_DIRS CACHE PATH "Boost include dirs")

set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/share/pixmaps\\\\vle.ico")
set(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/share/pixmaps\\\\vle.ico")
set(CPACK_NSIS_MENU_LINKS "${VLE_SHARE_DIRS}/doc/vle.chm" "VLE API" "http://www.vle-project.org" "VLE Web Site")
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\vle.exe")
set(CPACK_NSIS_DISPLAY_NAME "VLE - Virtual Laboratory Environment")
set(CPACK_NSIS_HELP_LINK "http://www.vle-project.org")
set(CPACK_NSIS_URL_INFO_ABOUT "http://www.vle-project.org")
set(CPACK_NSIS_CONTACT "Gauthier Quesnel <quesnel@users.sourceforge.net>")
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_MODIFY_PATH ON)


if (WITH_WIN32_INSTALLER)
  
  set(CPACK_MODULE_PATH "${PROJECT_SOURCE_DIR}/share")
  if (NOT WIN32) 
    message(FATAL_ERROR "win32 installer: cannot be prepared for others than WIN32 host")
  endif()
  find_path(MINGW_INSTALL "cmake.exe")
  get_filename_component(MINGW_INSTALL ${MINGW_INSTALL} DIRECTORY)
  get_filename_component(MINGW_INSTALL ${MINGW_INSTALL}  ABSOLUTE)
  get_filename_component(CMAKE_INSTALL_PREFIX_ABS ${CMAKE_INSTALL_PREFIX} ABSOLUTE)
  if (MINGW_INSTALL STREQUAL CMAKE_INSTALL_PREFIX_ABS)
    message(FATAL_ERROR "win32 installer: CMAKE_INSTALL_PREFIX cannot be equal to mingw path")
  endif()
  
  

  
  ##deployt qt
  #find_program(WINDEPLOYQT_CMD windeployqt)
  #list(APPEND WINDEPLOYQT_ARGS --release)
  #list(APPEND WINDEPLOYQT_ARGS --compiler-runtime)
  #list(APPEND WINDEPLOYQT_ARGS --qthelp)
  #list(APPEND WINDEPLOYQT_ARGS --network)
  #list(APPEND WINDEPLOYQT_ARGS --sql)
  #list(APPEND WINDEPLOYQT_ARGS --xml)
  #list(APPEND WINDEPLOYQT_ARGS --printsupport)
  #add_custom_target(windeployqt ALL ${WINDEPLOYQT_CMD}  $<TARGET_FILE:gvle>
  #      --dir ${CMAKE_INSTALL_PREFIX}/bin 
  #      --libdir ${CMAKE_INSTALL_PREFIX}/bin
  #      ${WINDEPLOYQT_ARGS}
  #		DEPENDS gvle
  #      COMMENT "Preparing Qt runtime dependencies") 
  
  install(DIRECTORY "${MINGW_INSTALL}/bin/" DESTINATION bin 
    FILES_MATCHING PATTERN "Qt5Core*.dll")
  install(DIRECTORY "${MINGW_INSTALL}/bin/" DESTINATION bin 
    FILES_MATCHING PATTERN "Qt5Gui*.dll")
  install(DIRECTORY "${MINGW_INSTALL}/bin/" DESTINATION bin 
    FILES_MATCHING PATTERN "Qt5Xml.dll")
  install(DIRECTORY "${MINGW_INSTALL}/bin/" DESTINATION bin 
    FILES_MATCHING PATTERN "Qt5Xmld.dll")
  install(DIRECTORY "${MINGW_INSTALL}/bin/" DESTINATION bin 
    FILES_MATCHING PATTERN "Qt5Help*.dll")
  install(DIRECTORY "${MINGW_INSTALL}/bin/" DESTINATION bin 
    FILES_MATCHING PATTERN "Qt5Network*.dll")
  install(DIRECTORY "${MINGW_INSTALL}/bin/" DESTINATION bin 
    FILES_MATCHING PATTERN "Qt5Sql*.dll")
  install(DIRECTORY "${MINGW_INSTALL}/bin/" DESTINATION bin 
    FILES_MATCHING PATTERN "Qt5Widgets*.dll")
  install(DIRECTORY "${MINGW_INSTALL}/bin/" DESTINATION bin 
    FILES_MATCHING PATTERN "Qt5CLucene*.dll")
  install(DIRECTORY "${MINGW_INSTALL}/bin/" DESTINATION bin 
    FILES_MATCHING PATTERN "Qt5PrintSupport*.dll")
  install(DIRECTORY "${MINGW_INSTALL}/bin/" DESTINATION bin 
    PATTERN "Qt*.dll" EXCLUDE
    PATTERN "windeployqt.exe" EXCLUDE
    PATTERN "designer.exe" EXCLUDE
    PATTERN "assistant.exe" EXCLUDE
    PATTERN "cmake-gui.exe" EXCLUDE)
  install(DIRECTORY "${MINGW_INSTALL}/lib/" DESTINATION lib 
    FILES_MATCHING PATTERN "libQt5Core*.dll.a")
  install(DIRECTORY "${MINGW_INSTALL}/lib/" DESTINATION lib 
    FILES_MATCHING PATTERN "libQt5Gui*.dll.a")
  install(DIRECTORY "${MINGW_INSTALL}/lib/" DESTINATION lib 
    FILES_MATCHING PATTERN "libQt5Xml.dll.a")
  install(DIRECTORY "${MINGW_INSTALL}/lib/" DESTINATION lib 
    FILES_MATCHING PATTERN "libQt5Xmld.dll.a")
  install(DIRECTORY "${MINGW_INSTALL}/lib/" DESTINATION lib 
    FILES_MATCHING PATTERN "libQt5Help*.dll.a")
  install(DIRECTORY "${MINGW_INSTALL}/lib/" DESTINATION lib 
    FILES_MATCHING PATTERN "libQt5Network*.dll.a")
  install(DIRECTORY "${MINGW_INSTALL}/lib/" DESTINATION lib 
    FILES_MATCHING PATTERN "libQt5Sql*.dll.a")
  install(DIRECTORY "${MINGW_INSTALL}/lib/" DESTINATION lib 
    FILES_MATCHING PATTERN "libQt5Widgets*.dll.a")
  install(DIRECTORY "${MINGW_INSTALL}/lib/" DESTINATION lib 
    FILES_MATCHING PATTERN "libQt5CLucene*.dll.a")
  install(DIRECTORY "${MINGW_INSTALL}/lib/" DESTINATION lib 
    FILES_MATCHING PATTERN "libQt5PrintSupport*.dll.a")
  install(DIRECTORY "${MINGW_INSTALL}/lib/" DESTINATION lib
    PATTERN "libQt5*" EXCLUDE
    PATTERN "libboost*" EXCLUDE
    PATTERN "terminfo" EXCLUDE)
  install(DIRECTORY "${MINGW_INSTALL}/include/QtCore" DESTINATION include)
  install(DIRECTORY "${MINGW_INSTALL}/include/QtGui" DESTINATION include)
  install(DIRECTORY "${MINGW_INSTALL}/include/QtXml" DESTINATION include)
  install(DIRECTORY "${MINGW_INSTALL}/include/QtHelp" DESTINATION include)
  install(DIRECTORY "${MINGW_INSTALL}/include/QtNetwork" DESTINATION include)
  install(DIRECTORY "${MINGW_INSTALL}/include/QtSql" DESTINATION include)
  install(DIRECTORY "${MINGW_INSTALL}/include/QtWidgets" DESTINATION include)
  install(DIRECTORY "${MINGW_INSTALL}/include/QtCLucene" DESTINATION include)
  install(DIRECTORY "${MINGW_INSTALL}/include/" DESTINATION include
    PATTERN "Qt*" EXCLUDE
    PATTERN "boost" EXCLUDE)
  install(DIRECTORY "${MINGW_INSTALL}/share/" DESTINATION share 
    PATTERN "qt5/examples" EXCLUDE
    PATTERN "qt5/doc" EXCLUDE
    PATTERN "qt5/qml" EXCLUDE
    PATTERN "doc" EXCLUDE
    PATTERN "man" EXCLUDE
    PATTERN "gtk-doc" EXCLUDE
    PATTERN "terminfo" EXCLUDE)

  
  
  install(DIRECTORY "${MINGW_INSTALL}/i686-w64-mingw32/" DESTINATION i686-w64-mingw32)
  install(DIRECTORY "${MINGW_INSTALL}/var/" DESTINATION var)
  install(DIRECTORY "${MINGW_INSTALL}/etc/" DESTINATION etc)
  install(DIRECTORY "${MINGW_INSTALL}/ssl/" DESTINATION ssl)
  install(DIRECTORY "${MINGW_INSTALL}/sbin/" DESTINATION sbin)

endif()

#TODO commands for improving NSIS script
#STRING(REPLACE "\\" "\\\\" VLE_MINGW_PATH_WIN ${VLE_MINGW_PATH}) 
#set(CPACK_NSIS_DEFINES "!define QT_DEPLOY '${VLE_MINGW_PATH_WIN}\\\\bin\\\\windeployqt.exe ${VLE_MINGW_PATH_WIN}\\\\bin\\\\gvle.exe --libdir @CPACK_TEMPORARY_DIRECTORY@\\\\bin'")



include(CPack)
