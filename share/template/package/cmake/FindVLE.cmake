# FindVLE.cmake
# =============
#
# Try to find VLE
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
#  VLE_FOUND             - The system has VLE
#  VLE_INCLUDE_DIRS      - The VLE include directory
#  VLE_LIBRARY_DIRS      - Directories containing libraries to link
#  VLE_LIBRARIES         - Link these to use shared libraries of VLE
#
# CMake variables used by this module:
#  
#  VLE_DEBUG             - If true, prints debug traces
#                          (default OFF)
#  VLE_USING_CMAKE       - If true, on windows, use cmake for finding VLE,
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
#   FindVLE will try to find VLE and the gtkmm dependencies,
#   only some boost libraries are not included into the libraries given
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
# find_package(VLE REQUIRED)
#
#=============================================================================

#
# Set default behavior of find vle
#

if (WIN32)
  if (DEFINED VLE_USING_CMAKE)
     set (_find_vle_using_cmake ${VLE_USING_CMAKE})
  else (DEFINED VLE_USING_CMAKE)
     set (_find_vle_using_cmake 1)
  endif (DEFINED VLE_USING_CMAKE)
else (WIN32)
  set (_find_vle_using_cmake 0)
endif (WIN32)

if (DEFINED VLE_DEBUG)
  set (_vle_debug  ${VLE_DEBUG})
else ()
  set (_vle_debug 0)
endif ()


#
# Find VLE
#

if (${_find_vle_using_cmake})
  find_path(_vle_base_include zlib.h PATHS
    $ENV{VLE_BASEPATH}/include
    ${VLE_BASEPATH_LOCAL}/include
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;]/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE 1.1.0;]/include")

  find_path(_vle_base_bin zlib1.dll PATHS
    $ENV{VLE_BASEPATH}/bin
    ${VLE_BASEPATH_LOCAL}/bin
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE 1.1.0;]/bin")

  find_path(_vle_base_lib libz.dll.a PATHS
    $ENV{VLE_BASEPATH}/lib
    ${VLE_BASEPATH_LOCAL}/lib
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/lib"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;]/lib"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE 1.1.0;]/lib")

  if (${_vle_debug})
    message (" vle_debug _vle_base_include ${_vle_base_include}")
    message (" vle_debug _vle_base_bin ${_vle_base_bin}")
    message (" vle_debug _vle_base_lib ${_vle_base_lib}")
  endif ()

  if(NOT _vle_base_include OR NOT _vle_base_bin OR NOT _vle_base_lib)
     message (FATAL_ERROR "Missing vle dependencies")
  endif ()

  set(VLE_INCLUDE_DIRS
    ${_vle_base_include}/vle-1.1;
    ${_vle_base_include}; ${_vle_base_include}/libxml2;
    ${_vle_base_include}/glibmm-2.4;${_vle_base_lib}/glibmm-2.4/include;
    ${_vle_base_include}/sigc++-2.0;${_vle_base_lib}/sigc++-2.0/include;
    ${_vle_base_include}/glib-2.0;${_vle_base_lib}/glib-2.0/include)

  set(VLE_LIBRARY_DIRS
    ${_vle_base_bin};${_vle_base_lib})

  set (VLE_LIBRARIES
    vle-1.1 xml2 glibmm-2.4 gobject-2.0 sigc-2.0 gthread-2.0 glib-2.0 intl)
else (${_find_vle_using_cmake})
  find_package(PkgConfig REQUIRED)
  PKG_CHECK_MODULES(VLE vle-1.1)
endif (${_find_vle_using_cmake})


# handle the QUIETLY and REQUIRED arguments and set VLE_FOUND to TRUE if all
# listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VLE REQUIRED_VARS VLE_INCLUDE_DIRS
                        VLE_LIBRARIES)

if (${_vle_debug})
  message (" vle_debug VLE_INCLUDE_DIRS ${VLE_INCLUDE_DIRS}")
  message (" vle_debug VLE_LIBRARY_DIRS ${VLE_LIBRARY_DIRS}")
  message (" vle_debug VLE_LIBRARIES ${VLE_LIBRARIES}")
endif ()

#mark_as_advanced(VLE_INCLUDE_DIRS VLE_LIBRARIES VLE_LIBRARY_DIRS)
