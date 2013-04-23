include(InstallRequiredSystemLibraries)

if (WIN32)
  set(CPACK_GENERATOR "ZIP;NSIS")
  set(CPACK_SOURCE_GENERATOR "ZIP")
endif (WIN32)

if (UNIX)
  set(CPACK_GENERATOR "TGZ;DEB;RPM")
  set(CPACK_SOURCE_GENERATOR "ZIP;TGZ;TBZ2")
endif (UNIX)

if (APPLE)
  set(CPACK_GENERATOR "OSXX11")
endif (APPLE)

set(CPACK_STRIP_FILES "1")
set(CPACK_PACKAGE_NAME "VLE")
set(CPACK_PACKAGE_VENDOR "VLE Development Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "VLE - Virtual Laboratory Environment")
set(CPACK_PACKAGE_CONTACT "Gauthier Quesnel <quesnel@users.sourceforge.net>")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")

if (NOT CMAKE_SYSTEM_PROCESSOR)
  if (NOT CMAKE_HOST_SYSTEM_PROCESSOR)
    set(CPACK_PACKAGE_FILE_NAME "${VLE_NAME_COMPLETE}-${CMAKE_SYSTEM_NAME}")
  else ()
    set(CPACK_PACKAGE_FILE_NAME "${VLE_NAME_COMPLETE}-${CMAKE_HOST_SYSTEM_PROCESSOR}")
  endif ()
else ()
  set(CPACK_PACKAGE_FILE_NAME "${VLE_NAME_COMPLETE}-${CMAKE_SYSTEM_PROCESSOR}")
endif ()

set(CPACK_PACKAGE_FILE_NAME "${VLE_NAME_COMPLETE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_PACKAGE_VERSION_MAJOR "${VLE_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VLE_MINOR}")

if ("${VLE_EXTRA}" STREQUAL "")
  set(CPACK_PACKAGE_VERSION_PATCH "${VLE_PATCH}")
else ()
  set(CPACK_PACKAGE_VERSION_PATCH "${VLE_PATCH}-${VLE_EXTRA}")
endif ()

set(CPACK_PACKAGE_INSTALL_DIRECTORY ${VLE_NAME_COMPLETE})
set(CPACK_PACKAGE_EXECUTABLES "vle" "VLE" "gvle" "GVLE")

# CPack source configuration
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${VLE_NAME_COMPLETE})
set(CPACK_SOURCE_IGNORE_FILES "\\\\.swp$;/\\\\.gitignore;/build/;/\\\\.git/")

# CPack DEB configuration
set(CPACK_DEBIAN_PACKAGE_NAME ${VLE_NAME}-${VLE_MAJOR}.${VLE_MINOR})
set(CPACK_DEBIAN_PACKAGE_VERSION ${VLE_COMPLETE})
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
set(CPACK_DEBIAN_PACKAGE_BUILDS_DEPENDS "")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Gauthier Quesnel <quesnel@users.sourceforge.net>")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "VLE, a framework for multi-modeling, simulation and analysis of complex dynamical systems.")
set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")

#CPack RPM configuration
set(CPACK_RPM_PACKAGE_SUMMARY "VLE, a framework for multi-modeling, simulation and analysis of complex dynamical systems.")
set(CPACK_RPM_PACKAGE_NAME ${VLE_NAME}-${VLE_MAJOR}.${VLE_MINOR})
set(CPACK_RPM_PACKAGE_VERSION ${VLE_MAJOR}.${VLE_MINOR}.${VLE_PATCH})
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_VENDOR "Gauthier Quesnel <quesnel@users.sourceforge.net>")
set(CPACK_RPM_PACKAGE_LICENSE "GNU General Public License (GPL)")
set(CPACK_RPM_PACKAGE_DESCRIPTION "VLE, a framework for multi-modeling, simulation and analysis of complex dynamical systems.")

# CPack NSIS configuration
if (CPACK_GENERATOR MATCHES "NSIS")

  set(CMAKE_MODULE_PATH "share")
  set(VLE_MINGW_PATH CACHE PATH "Mingw Boost directory")
  set(VLE_BOOST_INCLUDE_PATH CACHE PATH "Boost include path")
  set(VLE_BOOST_LIBRARIES_PATH CACHE PATH "Boost libraries path")
  set(VLE_GTK_PATH CACHE PATH "Gtk directory")
  set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/share)

  install(FILES "${VLE_MINGW_PATH}/bin\\\\mingwm10.dll" DESTINATION bin)

  install(DIRECTORY "${VLE_BOOST_INCLUDE_PATH}" DESTINATION include)
  install(DIRECTORY "${VLE_BOOST_LIBRARIES_PATH}/" DESTINATION bin
    FILES_MATCHING PATTERN "libboost*.dll")
  install(DIRECTORY "${VLE_BOOST_LIBRARIES_PATH}/" DESTINATION lib
    FILES_MATCHING PATTERN "libboost*.a")

  install(FILES "${VLE_GTK_PATH}/bin\\\\pkg-config.exe" DESTINATION bin)
  install(DIRECTORY "${VLE_GTK_PATH}/etc\\\\gtk-2.0" DESTINATION etc)
  install(DIRECTORY "${VLE_GTK_PATH}/etc\\\\pango" DESTINATION etc)
  install(DIRECTORY "${VLE_GTK_PATH}/share\\\\doc" DESTINATION share)
  install(DIRECTORY "${VLE_GTK_PATH}/share\\\\locale" DESTINATION share)
  install(DIRECTORY "${VLE_GTK_PATH}/share\\\\themes" DESTINATION share)
  install(DIRECTORY "${VLE_GTK_PATH}/include/" DESTINATION include)
  install(DIRECTORY "${VLE_GTK_PATH}/redist/" DESTINATION bin)
  install(DIRECTORY "${VLE_GTK_PATH}/lib/" DESTINATION lib
    PATTERN "*.def" EXCLUDE
    PATTERN "*.lib" EXCLUDE)

  set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/share/pixmaps\\\\vle.ico")
  set(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/share/pixmaps\\\\vle.ico")
  set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/share/pixmaps\\\\logo.bmp")
  set(CPACK_NSIS_MENU_LINKS "${VLE_SHARE_DIRS}/doc/vle.chm" "VLE API" "http://www.vle-project.org" "VLE Web Site")
  set(CPACK_CREATE_DESKTOP_LINKS gvle)
  set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\vle.exe")
  set(CPACK_NSIS_DISPLAY_NAME "VLE - Virtual Laboratory Environment")
  set(CPACK_NSIS_HELP_LINK "http://www.vle-project.org")
  set(CPACK_NSIS_URL_INFO_ABOUT "http://www.vle-project.org")
  set(CPACK_NSIS_CONTACT "Gauthier Quesnel <quesnel@users.sourceforge.net>")
  set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} ${VLE_VERSION_SHORT}.0")
  set(CPACK_NSIS_MODIFY_PATH ON)
endif ()

include(CPack)

# vim:tw=0:ts=8:tw=0:sw=2:sts=2
