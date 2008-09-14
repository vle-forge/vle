CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
PROJECT(VLE CXX C)
ENABLE_TESTING()

SET(VLE_MAJOR 0)
SET(VLE_MINOR 6)
SET(VLE_PATCH 0)
SET(VLE_EXTRA "-dev")
SET(VLE_VERSION "${VLE_MAJOR}.${VLE_MINOR}.${VLE_PATCH}")
SET(VLE_VERSION_SHORT "${VLE_MAJOR}.${VLE_MINOR}")
SET(VLE_NAME "vle")
SET(VLE_NAME_VERSION "${VLE_NAME}-${VLE_VERSION}")
SET(VLE_COMPLETE "${VLE_MAJOR}.${VLE_MINOR}.${VLE_PATCH}${VLE_EXTRA}")
SET(VLE_NAME_COMPLETE "${VLE_NAME}-${VLE_COMPLETE}")

#
 # Installation variables
#

SET(VLE_LIBRARY_DIRS "lib/${VLE_NAME_VERSION}")
SET(VLE_INCLUDE_DIRS "include/${VLE_NAME_VERSION}/vle")
SET(VLE_INCLUDE_DIR "include/${VLE_NAME_VERSION}")
SET(VLE_MANPAGE_PATH "share/man/man1")
SET(VLE_SHARE_DIRS "share/${VLE_NAME_VERSION}")

#
 # Append the library version information to the library target
 # properties.
#

SET (VLE_LIBRARY_PROPERTIES ${VLE_LIBRARY_PROPERTIES} VERSION "${VLE_VERSION}" SOVERSION "${VLE_MAJOR}.${VLE_MINOR}")
SET (VLE_APP_PROPERTIES ${VLE_APP_PROPERTIES} VERSION "${VLE_VERSION}")

#
 # CPack configuration
#

IF(CMAKE_MAJOR_VERSION MATCHES "2" AND CMAKE_MINOR_VERSION MATCHES "6")
  IF (WIN32)
    SET(CPACK_GENERATOR "NSIS")
    SET(CPACK_SOURCE_GENERATOR "ZIP")
  ENDIF (WIN32)

  IF (UNIX)
    SET(CPACK_GENERATOR "STGZ;TGZ;TBZ2;TZ;DEB;RPM")
    SET(CPACK_SOURCE_GENERATOR "TGZ;TBZ2;TGZ;TZ")
  ENDIF (UNIX)

  IF (APPLE)
    SET(CPACK_GENERATOR "OSXX11")
  ENDIF (APPLE)

  SET(CPACK_PACKAGE_NAME "VLE")
  SET(CPACK_PACKAGE_VENDOR "VLE Development Team")
  SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "VLE - Virtual Laboraory Environment")
  SET(CPACK_PACKAGE_CONTACT "Gauthier Quesnel <quesnel@users.sourceforge.net>")
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README")
  SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
  SET(CPACK_PACKAGE_FILE_NAME "${VLE_NAME_COMPLETE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
  SET(CPACK_PACKAGE_VERSION_MAJOR "${VLE_MAJOR}")
  SET(CPACK_PACKAGE_VERSION_MINOR "${VLE_MINOR}")
  IF (${VLE_EXTRA} STREQUAL "")
    SET(CPACK_PACKAGE_VERSION_PATCH "${VLE_PATCH}")
  ELSE (${VLE_EXTRA} STREQUAL "")
    SET(CPACK_PACKAGE_VERSION_PATCH "${VLE_PATCH}${VLE_EXTRA}")
  ENDIF (${VLE_EXTRA} STREQUAL "")
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY ${VLE_NAME_COMPLETE})
  SET(CPACK_PACKAGE_EXECUTABLES "vle" "VLE" "oov" "OOV" "eov" "EOV" "gvle" "GVLE")

  # CPack source configuration
  SET(CPACK_SOURCE_PACKAGE_FILE_NAME ${VLE_NAME_COMPLETE}-sources)
  SET(CPACK_SOURCE_IGNORE_FILES "\\\\.swp$;/\\\\.gitignore;/build/;/\\\\.git/")

  # CPack DEB configuration
  SET(CPACK_DEBIAN_PACKAGE_NAME ${VLE_NAME})
  SET(CPACK_DEBIAN_PACKAGE_VERSION ${VLE_COMPLETE})
  SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
  SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libsigc++-2.0-0c2a, libglibmm-2.4-1c2a, libxml++2.6-2, libcairomm-1.0-1")
  SET(CPACK_DEBIAN_PACKAGE_BUILDS_DEPENDS "debhelper (>= 5.0.0), cmake (>= 2.4), gcc (>= 4:3.2.2-0), g++ (>= 4:3.2.2-0), libsigc++-2.0-dev, libglibmm-2.4-dev, libxml++2.6-dev, libcairomm-1.0-dev, libboost-dev, pkg-config")
  SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Gauthier Quesnel <quesnel@users.sourceforge.net>")
  SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "VLE, an environment of modeling and simulation")
  SET(CPACK_DEBIAN_PACKAGE_SECTION "libs")
  SET(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")

  # CPack RPM configuration
  SET(CPACK_RPM_PACKAGE_SUMMARY "VLE, an environment of modeling and simulation")
  SET(CPACK_RPM_PACKAGE_NAME ${VLE_NAME})
  SET(CPACK_RPM_PACKAGE_VERSION ${VLE_MAJOR}.${VLE_MINOR}.${VLE_PATCH})
  SET(CPACK_RPM_PACKAGE_ARCHITECTURE i386)
  SET(CPACK_RPM_PACKAGE_RELEASE "1")
  SET(CPACK_RPM_PACKAGE_VENDOR "VLE Development Team")
  SET(CPACK_RPM_PACKAGE_LICENSE "GNU General Public License (GPL)")
  SET(CPACK_RPM_PACKAGE_DESCRIPTION "VLE, an environment of modeling and simulation")

  # CPack NSIS configuration
  IF (CPACK_GENERATOR MATCHES "NSIS")
    SET(CMAKE_MODULE_PATH "share")
    SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/share)
    FILE(TO_CMAKE_PATH "${BOOST_INCLUDEDIR}" BOOST_INCLUDEDIR_NEW)
    FILE(TO_CMAKE_PATH "${BOOST_LIBRARYDIR}" BOOST_LIBRARYDIR_NEW)
    INSTALL(DIRECTORY "${BOOST_INCLUDEDIR_NEW}" DESTINATION include)
    INSTALL(DIRECTORY "${BOOST_LIBRARYDIR_NEW}/" DESTINATION bin FILES_MATCHING PATTERN "*.dll")
    INSTALL(DIRECTORY "${BOOST_LIBRARYDIR_NEW}/" DESTINATION lib FILES_MATCHING PATTERN "*.lib")

    SET(GTK_BASE "c:\\\\gtk")
    INSTALL(FILES "${GTK_BASE}/bin\\\\pkg-config.exe" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libatk-1.0-0.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libglib-2.0-0.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libgmodule-2.0-0.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libgobject-2.0-0.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libgthread-2.0-0.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libcairo-2.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libpango-1.0-0.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libpangocairo-1.0-0.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libpangoft2-1.0-0.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libpangowin32-1.0-0.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libgdk-win32-2.0-0.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libgdk_pixbuf-2.0-0.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libgtk-win32-2.0-0.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libfreetype-6.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libfontconfig-1.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libglade-2.0-0.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\librsvg-2-2.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libxml2.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libsigc-2.0-0.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libcairomm-1.0-1.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libglibmm-2.4-1.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libglibmm_generate_extra_defs-2.4-1.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libatkmm-1.6-1.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libgdkmm-2.4-1.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libgtkmm-2.4-1.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libpangomm-1.4-1.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libglademm-2.4-1.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\sigc-2.0.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\zlib1.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\iconv.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\intl.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libpng12.dll" DESTINATION bin)
    INSTALL(FILES "${GTK_BASE}/bin\\\\libxml++-2.6-2.dll" DESTINATION bin)

    INSTALL(FILES "${GTK_BASE}/include\\\\iconv.h" DESTINATION include)
    INSTALL(FILES "${GTK_BASE}/include\\\\zconf.h" DESTINATION include)
    INSTALL(FILES "${GTK_BASE}/include\\\\zlib.h" DESTINATION include)
    INSTALL(FILES "${GTK_BASE}/include\\\\png.h" DESTINATION include)
    INSTALL(FILES "${GTK_BASE}/include\\\\pngconf.h" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\glib-2.0" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\gtk-2.0" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\cairo" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\pango-1.0" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\libglade-2.0" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\gtkglext-1.0" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\libxml2" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\libpng12" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\atk-1.0" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\freetype2" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\fontconfig" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\sigc++-2.0" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\cairomm-1.0" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\glibmm-2.4" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\atkmm-1.6" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\gtkmm-2.4" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\gdkmm-2.4" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\pangomm-1.4" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\libglademm-2.4" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\librsvg-2" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\libxml2" DESTINATION include)
    INSTALL(DIRECTORY "${GTK_BASE}/include\\\\libxml++-2.6" DESTINATION include)

    INSTALL(DIRECTORY "${GTK_BASE}/lib\\\\pango" DESTINATION lib)
    INSTALL(DIRECTORY "${GTK_BASE}/lib\\\\gtk-2.0" DESTINATION lib)
    INSTALL(DIRECTORY "${GTK_BASE}/lib\\\\libglade" DESTINATION lib)
    INSTALL(DIRECTORY "${GTK_BASE}/lib\\\\glib-2.0" DESTINATION lib)
    INSTALL(DIRECTORY "${GTK_BASE}/lib\\\\pkgconfig" DESTINATION lib)
    INSTALL(DIRECTORY "${GTK_BASE}/lib\\\\gtkglext-1.0" DESTINATION lib)
    INSTALL(DIRECTORY "${GTK_BASE}/lib\\\\gtkmm-2.4" DESTINATION lib)
    INSTALL(DIRECTORY "${GTK_BASE}/lib\\\\gdkmm-2.4" DESTINATION lib)
    INSTALL(DIRECTORY "${GTK_BASE}/lib\\\\glibmm-2.4" DESTINATION lib)
    INSTALL(DIRECTORY "${GTK_BASE}/lib\\\\sigc++-2.0" DESTINATION lib)
    INSTALL(DIRECTORY "${GTK_BASE}/lib\\\\libxml++-2.6" DESTINATION lib)
    INSTALL(DIRECTORY "${GTK_BASE}/lib\\\\libglademm-2.4" DESTINATION lib)

    INSTALL(FILES "${GTK_BASE}/lib\\\\libglib-2.0.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libgmodule-2.0.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libgobject-2.0.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libgthread-2.0.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libgdk-win32-2.0.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libgdk_pixbuf-2.0.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libgtk-win32-2.0.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libcairo.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libpango-1.0.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libpangocairo-1.0.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libpangoft2-1.0.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libpangowin32-1.0.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libglade-2.0.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libiconv.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libintl.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libpng.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libpng12.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libatk-1.0.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libfreetype.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libfontconfig.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\librsvg-2.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libxml2.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libsigc-2.0.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libcairomm-1.0.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libglibmm-2.4.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libglibmm_generate_extra_defs-2.4.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libatkmm-1.6.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libgdkmm-2.4.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libgtkmm-2.4.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libpangomm-1.4.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libglademm-2.4.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libxml++-2.6.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\iconv.lib" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\intl.lib" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\sigc-2.0.lib" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\libz.dll.a" DESTINATION lib)
    INSTALL(FILES "${GTK_BASE}/lib\\\\z.lib" DESTINATION lib)

    INSTALL(DIRECTORY "${GTK_BASE}/etc\\\\fonts" DESTINATION etc)
    INSTALL(DIRECTORY "${GTK_BASE}/etc\\\\gtk-2.0" DESTINATION etc)
    INSTALL(DIRECTORY "${GTK_BASE}/etc\\\\pango" DESTINATION etc)

    SET(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/share/pixmaps\\\\vle.ico")
    SET(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/share/pixmaps\\\\vle.ico")
    SET(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/share/pixmaps\\\\logo.bmp")
    SET(CPACK_NSIS_MENU_LINKS
      "${VLE_SHARE_DIRS}/doc/html/index.html" "VLE API"
      "http://vle.univ-littoral.fr" "VLE Web Site")
    SET(CPACK_CREATE_DESKTOP_LINKS "GVLE" "VLE")
    SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\vle.exe")
    SET(CPACK_NSIS_DISPLAY_NAME "VLE - Virtual Laboraory Environment")
    SET(CPACK_NSIS_HELP_LINK "http://vle.univ-littoral.fr")
    SET(CPACK_NSIS_URL_INFO_ABOUT "http://vle.univ-littoral.fr")
    SET(CPACK_NSIS_CONTACT "Gauthier Quesnel <quesnel@users.sourceforge.net>")
    SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} ${VLE_VERSION_SHORT}.0")
    SET(CPACK_NSIS_MODIFY_PATH ON)
  ENDIF (CPACK_GENERATOR MATCHES "NSIS")

  INCLUDE(CPack)
ENDIF(CMAKE_MAJOR_VERSION MATCHES "2" AND CMAKE_MINOR_VERSION MATCHES "6")

#
 # Debug mode
#

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -pedantic")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS}")

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

#
 # Manage the Win32 port.
#

IF(WIN32)
  SET(OS_SPECIFIC_LIBRARIES wsock32 netapi32 z intl iconv)
ELSE(WIN32)
  SET(OS_SPECIFIC_LIBRARIES)
ENDIF(WIN32)

#
 # Modules
#

INCLUDE(CheckIncludeFileCXX)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckLibraryExists)
INCLUDE(CMakeDetermineCCompiler)

#
 # Check libraries with pkgconfig
#

FIND_PACKAGE(PkgConfig)
IF (NOT PkgConfig_FOUND)
  SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
  FIND_PACKAGE(PkgConfig REQUIRED)
ENDIF (NOT PkgConfig_FOUND)

# FIXME use old-style link directories for now
IF (COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 OLD)
ENDIF (COMMAND CMAKE_POLICY)

PKG_CHECK_MODULES(GLIBMM REQUIRED glibmm-2.4)
PKG_CHECK_MODULES(GTHREAD REQUIRED gthread-2.0)
PKG_CHECK_MODULES(LIBXMLPP REQUIRED libxml++-2.6)
PKG_CHECK_MODULES(CAIROMM cairomm-1.0>=1.2)
PKG_CHECK_MODULES(GLADEMM libglademm-2.4)
PKG_CHECK_MODULES(GTKMM gtkmm-2.4)

OPTION(WITH_CAIRO "use the cairomm to build output plugin [default: on]" ON)
IF (WITH_CAIRO)
  IF (CAIROMM_FOUND)
    SET(HAVE_CAIRO 1 CACHE INTERNAL "" FORCE)
  ENDIF (CAIROMM_FOUND)
ENDIF (WITH_CAIRO)

OPTION(WITH_EOV "use the gtkmm and glademm to build eov [default: on]" ON)
IF (WITH_EOV AND GLADEMM_FOUND AND GTKMM_FOUND)
  SET(HAVE_EOV 1 CACHE INTERNAL "" FORCE)
ENDIF (WITH_EOV AND GLADEMM_FOUND AND GTKMM_FOUND)

OPTION(WITH_GVLE "use the gtkmm and glademm to build gvle [default: on]" ON)
IF (WITH_GVLE AND GLADEMM_FOUND AND GTKMM_FOUND)
  SET(HAVE_GVLE 1 CACHE INTERNAL "" FORCE)
ENDIF (WITH_GVLE AND GLADEMM_FOUND AND GTKMM_FOUND)

#
 # Test the libsqlxx dependencies
#

OPTION(WITH_SQLXX "with libsqlxx for the odbc driver [default: on]" ON)
IF (WITH_SQLXX)
  CHECK_INCLUDE_FILE_CXX(sqlxx.h HAVE_SQLXX)
  IF (HAVE_SQLXX)
    SET(LIBSQLXX_LIBRARIES "sqlxx")
  ELSE (HAVE_SQLXX)
    SET(LIBSQLXX_LIBRARIES)
  ENDIF (HAVE_SQLXX)
ENDIF (WITH_SQLXX)

#
 # Test the libboost header and libboost-text library.
#

OPTION(WITH_TEST "use the libboost test unit library [default: on]" ON)
IF(CMAKE_MAJOR_VERSION MATCHES "2" AND CMAKE_MINOR_VERSION MATCHES "6")
  SET(Boost_USE_STATIC_LIBS OFF)
  SET(Boost_USE_MULTITHREAD ON)
  IF (WITH_TEST)
    FIND_PACKAGE(Boost COMPONENTS unit_test_framework)
    IF (Boost_UNIT_TEST_FRAMEWORK_FOUND)
      SET(HAVE_UNITTESTFRAMEWORK 1 CACHE INTERNAL "" FORCE)
    ENDIF (Boost_UNIT_TEST_FRAMEWORK_FOUND)
  ELSE (WITH_TEST)
    FIND_PACKAGE(Boost)
  ENDIF (WITH_TEST)
ELSE(CMAKE_MAJOR_VERSION MATCHES "2" AND CMAKE_MINOR_VERSION MATCHES "6")
  FIND_PACKAGE(Boost)
  IF (Boost_FOUND)
    IF (WITH_TEST)
      FIND_LIBRARY(Boost_UNIT_TEST_FRAMEWORK_LIBRARY NAMES
        boost_unit_test_framework boost_unit_test_framework-mt PATHS
        Boost_LIBRARY_DIRS)
      IF (Boost_UNIT_TEST_FRAMEWORK_LIBRARY)
        SET(HAVE_UNITTESTFRAMEWORK 1 CACHE INTERNAL "" FORCE)
      ELSE (Boost_UNIT_TEST_FRAMEWORK_LIBRARY)
        SET(HAVE_UNITTESTFRAMEWORK 0 CACHE INTERNAL "" FORCE)
      ENDIF (Boost_UNIT_TEST_FRAMEWORK_LIBRARY)
    ELSE (WITH_TEST)
      SET(HAVE_UNITTESTFRAMEWORK 0 CACHE INTERNAL "" FORCE)
    ENDIF (WITH_TEST)
  ENDIF (Boost_FOUND)
ENDIF (CMAKE_MAJOR_VERSION MATCHES "2" AND CMAKE_MINOR_VERSION MATCHES "6")

#
 # Generate the config.h
#

CHECK_INCLUDE_FILE_CXX(cxxabi.h HAVE_GCC_ABI_DEMANGLE)
CHECK_INCLUDE_FILE_CXX(execinfo.h HAVE_EXECINFO_H)
CHECK_INCLUDE_FILE_CXX(signal.h HAVE_SIGNAL_H)
INCLUDE_DIRECTORIES(${VLE_BINARY_DIR})
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/config.h.in ${VLE_BINARY_DIR}/config.h)

#
 # Generate the vle.pc pkg-config file.
 # Build some variable to build a correct Win32 pkg config file with:
 # prefix=/target to detect the DIRNAME
 # cflag=-Iboost_1_34_1 for the directory
#

IF(UNIX AND NOT WIN32)
  SET(VLE_PKGCONFIG_PREFIXPATH "${CMAKE_INSTALL_PREFIX}")
  SET(VLE_PKGCONFIG_BOOSTINCLUDE_DIRS "${Boost_INCLUDE_DIRS}")
ELSE(UNIX AND NOT WIN32)
  SET(VLE_PKGCONFIG_PREFIXPATH "/target")
  GET_FILENAME_COMPONENT(BOOST_DIRNAME "${Boost_INCLUDE_DIRS}" NAME)
  SET(VLE_PKGCONFIG_BOOSTINCLUDE_DIRS "\${includedir}/${BOOST_DIRNAME}")
ENDIF(UNIX AND NOT WIN32)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/src/vle/vle.pc.in ${VLE_BINARY_DIR}/vle-${VLE_VERSION_SHORT}.pc @ONLY)
INSTALL(FILES ${VLE_BINARY_DIR}/vle-${VLE_VERSION_SHORT}.pc DESTINATION lib/pkgconfig)

IF (HAVE_EOV)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/src/vle/eov.pc.in ${VLE_BINARY_DIR}/eov-${VLE_VERSION_SHORT}.pc @ONLY)
  INSTALL(FILES ${VLE_BINARY_DIR}/eov-${VLE_VERSION_SHORT}.pc DESTINATION lib/pkgconfig)
ENDIF (HAVE_EOV)

#
 # Install the README
#

IF(UNIX)
  INSTALL(FILES "README" DESTINATION "${VLE_SHARE_DIRS}")
  INSTALL(FILES "COPYING" DESTINATION "${VLE_SHARE_DIRS}")
ELSE(UNIX)
  INSTALL(FILES "README" DESTINATION "${VLE_SHARE_DIRS}" RENAME "Readme.txt")
  INSTALL(FILES "COPYING" DESTINATION "${VLE_SHARE_DIRS}" RENAME "CopyRight.txt")
ENDIF(UNIX)

#
 # Browse the share subdirectory
#

ADD_SUBDIRECTORY(share)

#
 # Generating documentation with doxygen
#

OPTION(WITH_DOXYGEN "build the documentation with doxygen [default: off]" OFF)
IF (WITH_DOXYGEN)
  FIND_PACKAGE(Doxygen)
  IF (DOXYGEN)
    ADD_SUBDIRECTORY(doxygen)
  ELSE (DOXYGEN)
    MESSAGE(STATUS "WARNING: Doxygen not found - No reference manual create")
  ENDIF (DOXYGEN)
ENDIF(WITH_DOXYGEN)

#
 # Browse the src subdirectory
#

ADD_SUBDIRECTORY(src)

#
 # Status
#

MESSAGE(STATUS "- - - -")
MESSAGE(STATUS "${VLE_NAME_COMPLETE} configured successfully")
MESSAGE(STATUS "Using ${CMAKE_INSTALL_PREFIX} for installation")
MESSAGE(STATUS "Build type ${CMAKE_BUILD_TYPE}")
IF (HAVE_GCC_ABI_DEMANGLE)
  MESSAGE(STATUS "Build with GCC ABI Demangle...: yes")
ENDIF (HAVE_GCC_ABI_DEMANGLE)
IF (HAVE_SQLXX)
  MESSAGE(STATUS "Build with SQL................: yes")
ENDIF (HAVE_SQLXX)
IF (HAVE_EXECINFO_H)
  MESSAGE(STATUS "Build with execinfo.h.........: yes")
ENDIF (HAVE_EXECINFO_H)
IF (Boost_FOUND)
  MESSAGE(STATUS "Build with boost..............: yes")
ENDIF (Boost_FOUND)
IF (HAVE_UNITTESTFRAMEWORK)
  MESSAGE(STATUS "Build unit test...............: yes")
ENDIF (HAVE_UNITTESTFRAMEWORK)
IF (HAVE_EOV)
  MESSAGE(STATUS "Build with eov................: yes")
ENDIF (HAVE_EOV)
IF (HAVE_CAIRO)
  MESSAGE(STATUS "Build with cairo plugin.......: yes")
ENDIF (HAVE_CAIRO)
IF (HAVE_GVLE)
  MESSAGE(STATUS "Build with gvle...............: yes")
ENDIF (HAVE_GVLE)
MESSAGE(STATUS "- - - -")

# vim:tw=0:ts=8:tw=0:sw=2:sts=2
