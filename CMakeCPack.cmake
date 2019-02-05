include(InstallRequiredSystemLibraries)

# Common configuration

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
set(CPACK_PACKAGE_FILE_NAME "${VLE_NAME_COMPLETE}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${VLE_NAME_COMPLETE})
set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/share/pixmaps\\\\logo.bmp")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_EXECUTABLES "vle" "VLE" "gvle" "GVLE")
set(CPACK_STRIP_FILES "1")

# CPack source configuration

set(CPACK_SOURCE_PACKAGE_FILE_NAME ${VLE_NAME_COMPLETE})
set(CPACK_SOURCE_IGNORE_FILES "\\\\.swp$;/\\\\.gitignore;/build/;/\\\\.git/")

# CPack DEB configuration

set(CPACK_DEBIAN_PACKAGE_NAME ${VLE_NAME}-${VLE_MAJOR}.${VLE_MINOR})
set(CPACK_DEBIAN_PACKAGE_VERSION ${VLE_COMPLETE})
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
set(CPACK_DEBIAN_PACKAGE_BUILDS_DEPENDS "")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Gauthier Quesnel <gauthier.quesnel@inra.fr>")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "VLE, a framework for multi-modeling, simulation and analysis of complex dynamical systems.")
set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")

# CPack RPM configuration

set(CPACK_RPM_PACKAGE_SUMMARY "VLE, a framework for multi-modeling, simulation and analysis of complex dynamical systems.")
set(CPACK_RPM_PACKAGE_NAME ${VLE_NAME}-${VLE_MAJOR}.${VLE_MINOR})
set(CPACK_RPM_PACKAGE_VERSION ${VLE_MAJOR}.${VLE_MINOR}.${VLE_PATCH})
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_VENDOR "Gauthier Quesnel <gauthier.quesnel@inra.fr>")
set(CPACK_RPM_PACKAGE_LICENSE "GNU General Public License (GPL)")
set(CPACK_RPM_PACKAGE_DESCRIPTION "VLE, a framework for multi-modeling, simulation and analysis of complex dynamical systems.")

# CPack NSIS configuration

set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} ${VLE_ABI}.0")
set(CPACK_CREATE_DESKTOP_LINKS gvle)
set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/share/pixmaps\\\\vle.ico")
set(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/share/pixmaps\\\\vle.ico")
set(CPACK_NSIS_MENU_LINKS "${CMAKE_INSTALL_DATADIR}/vle-${VLE_ABI}/doc/vle.chm" "VLE API" "https://www.vle-project.org" "VLE Web Site")
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\vle.exe")
set(CPACK_NSIS_DISPLAY_NAME "VLE - Virtual Laboratory Environment")
set(CPACK_NSIS_HELP_LINK "https://www.vle-project.org")
set(CPACK_NSIS_URL_INFO_ABOUT "https://www.vle-project.org")
set(CPACK_NSIS_CONTACT "Gauthier Quesnel <gauthier.quesnel@inra.fr>")
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_MODIFY_PATH ON)

include(CPack)
