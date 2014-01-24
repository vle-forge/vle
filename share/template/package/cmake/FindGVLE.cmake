# FindGVLE.cmake
# ==============
#
# Try to find GVLE
#
# Copyright 2012-2014 INRA
# Gauthier Quesnel <quesnel@users.sourceforge.net>
# Ronan Trépos <ronan.trepos@toulouse.inra.fr>
#
# Distributed under the OS-approved BSD License (the "License");
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#
# Once done this will define:
#
#  GVLE_FOUND            - The system has GVLE
#  GVLE_INCLUDE_DIR      - The GVLE include directory
#  GVLE_LIBRARY_DIRS     - Directories containing libraries to link
#  GVLE_LIBRARIES        - Link these to use shared libraries of GVLE
#
# CMake variables used by this module:
#
#  GVLE_DEBUG            - If true, prints debug traces
#                          (default OFF)
#  GVLE_USING_CMAKE      - If true, on windows, use cmake for finding GVLE,
#                          else use pkgconfig
#                          (default ON)
#  VLE_BASEPATH_LOCAL    - cmake variable for base path of vle
#                          (default NOT_DEFINED)
#
# Environment variables used by this module:
#
#  VLE_BASEPATH          - environment variable for base path of vle
#
#=============================================================================
#
# Notes :
# ---------
#   FindGVLE will try to find GVLE, VLE and the gtkmm dependencies,
#   finding GVLE does not require to call FindVLE before
#
#=============================================================================
#
# Changelog
# ---------
#
# 1.0 Initial version.
#
# Usage
# -----
#
# find_package(GVLE REQUIRED)
#
#=============================================================================

#
# Set default behavior of find gvle
#

if (WIN32)
  if (DEFINED GVLE_USING_CMAKE)
     set (_find_gvle_using_cmake ${GVLE_USING_CMAKE})
  else ()
     set (_find_gvle_using_cmake 1)
  endif ()
else ()
  set (_find_gvle_using_cmake 0)
endif ()

if (DEFINED GVLE_DEBUG)
  set (_gvle_debug  ${GVLE_DEBUG})
else ()
  set (_gvle_debug 0)
endif ()

#
# Find GVLE
#

if (${_find_gvle_using_cmake})
  find_path(_gvle_base_include zlib.h PATHS
    $ENV{VLE_BASEPATH}/include
    ${VLE_BASEPATH_LOCAL}/include
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;]/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE 1.1.0;]/include")

  find_path(_gvle_base_bin zlib1.dll PATHS
    $ENV{VLE_BASEPATH}/bin
    ${VLE_BASEPATH_LOCAL}/bin
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE 1.1.0;]/bin")

  find_path(_gvle_base_lib libz.dll.a PATHS
    $ENV{VLE_BASEPATH}/lib
    ${VLE_BASEPATH_LOCAL}/lib
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/lib"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;]/lib"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE 1.1.0;]/lib")

  if (${_gvle_debug})
    message (" gvle_debug _gvle_base_include ${_gvle_base_include}")
    message (" gvle_debug _gvle_base_bin ${_gvle_base_bin}")
    message (" gvle_debug _gvle_base_lib ${_gvle_base_lib}")
  endif ()

  if(NOT _gvle_base_include OR NOT _gvle_base_bin OR NOT _gvle_base_lib)
     message (FATAL_ERROR "Missing gvle dependencies")
  endif ()

  set(GVLE_INCLUDE_DIRS
    ${_gvle_base_include}/vle-1.1; ${_gvle_base_include};
    ${_gvle_base_include}/gtkmm-2.4;${_gvle_base_lib}/gtkmm-2.4/include;
    ${_gvle_base_include}/cairomm-1.0;${_gvle_base_lib}/cairomm-1.0/include;
    ${_gvle_base_include}/atkmm-1.6;
    ${_gvle_base_include}/giomm-2.4;${_gvle_base_lib}/giomm-2.4/include;
    ${_gvle_base_include}/pangomm-1.4;${_gvle_base_lib}/pangomm-1.4/include;
    ${_gvle_base_include}/gdkmm-2.4;${_gvle_base_lib}/gdkmm-2.4/include;
    ${_gvle_base_include}/atk-1.0;
    ${_gvle_base_include}/glibmm-2.4;${_gvle_base_lib}/glibmm-2.4/include;
    ${_gvle_base_include}/glib-2.0;${_gvle_base_lib}/glib-2.0/include;
    ${_gvle_base_include}/sigc++-2.0;${_gvle_base_lib}/sigc++-2.0/include;
    ${_gvle_base_include}/pango-1.0;${_gvle_base_include}/cairo;
    ${_gvle_base_include}/freetype2;${_gvle_base_include}/libpgn14;
    ${_gvle_base_include}/gtk-2.0;${_gvle_base_lib}/gtk-2.0/include;
    ${_gvle_base_include}/gdk-pixbuf-2.0;
    ${_gvle_base_lib}/gdk-pixbuf-2.0/include;
    ${_gvle_base_include}/libxml2)

  set (GVLE_LIBRARY_DIRS
    ${_gvle_base_bin}; ${_gvle_base_lib};
    c:/devel/dist/win32/lipng-1.4.3-1/lib)

  set (GVLE_LIBRARIES
    gvle-1.1 gtkmm-2.4 vle-1.1 atkmm-1.6 gdkmm-2.4 giomm-2.4 pangomm-1.4
    gtk-win32-2.0 cairomm-1.0 gdk-win32-2.0 atk-1.0 gio-2.0 pangowin32-1.0
    gdi32 pangocairo-1.0 gdk_pixbuf-2.0 png14 pango-1.0 gmodule-2.0 cairo
    xml2 glibmm-2.4 gobject-2.0 sigc-2.0 gthread-2.0 glib-2.0 intl)
else () # find gvle using pkg-config
  find_package(PkgConfig REQUIRED)
  PKG_CHECK_MODULES(GVLE gvle-1.1)
endif ()

# handle the QUIETLY and REQUIRED arguments and set GVLE_FOUND to TRUE if all
# listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GVLE REQUIRED_VARS
  GVLE_INCLUDE_DIRS GVLE_LIBRARIES)

if (${_gvle_debug})
  message (" gvle_debug GVLE_INCLUDE_DIRS ${GVLE_INCLUDE_DIRS}")
  message (" gvle_debug GVLE_LIBRARY_DIRS ${GVLE_LIBRARY_DIRS}")
  message (" gvle_debug GVLE_LIBRARIES ${GVLE_LIBRARIES}")
endif ()

#mark_as_advanced(GVLE_INCLUDE_DIRS GVLE_LIBRARIES GVLE_LIBRARY_DIRS)
