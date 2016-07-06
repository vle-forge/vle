# FindGVLE.cmake
# ==============
#
# Try to find GVLE
#
# Copyright 2012-2016 INRA
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
#  VLE_ABI_VERSION       - gives the VLE version to search for (e.g 1.3 2.0)
#                          (REQUIRED)
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
#   FindGVLE will try to find GVLE, VLE and Qt dependencies,
#   finding GVLE does not require to call FindVLE before
#
#=============================================================================
#
# Usage
# -----
#
# set(VLE_ABI_VERSION 2.0)
# find_package(GVLE REQUIRED)
#
#=============================================================================

#
# Check VLE_ABI_VERSION
#

if (NOT DEFINED VLE_ABI_VERSION)
  message (FATAL_ERROR "Cmake variable VLE_ABI_VERSION is not set")
endif ()

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

  find_path(_gvle_base_include vle-${VLE_ABI_VERSION}/vle/gvle/gvle_widgets.h PATHS
    $ENV{VLE_BASEPATH}/include
    ${VLE_BASEPATH_LOCAL}/include
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${VLE_ABI_VERSION}.0;Path]/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${VLE_ABI_VERSION}.0;]/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE ${VLE_ABI_VERSION}.0;]/include"
        NO_DEFAULT_PATH)

  find_path(_gvle_base_bin gvle.exe PATHS
    $ENV{VLE_BASEPATH}/bin
    ${VLE_BASEPATH_LOCAL}/bin
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${VLE_ABI_VERSION}.0;Path]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${VLE_ABI_VERSION}.0;]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE ${VLE_ABI_VERSION}.0;]/bin"
        NO_DEFAULT_PATH)

  find_path(_gvle_base_lib libgvle-${VLE_ABI_VERSION}.dll.a PATHS
    $ENV{VLE_BASEPATH}/lib
    ${VLE_BASEPATH_LOCAL}/lib
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${VLE_ABI_VERSION}.0;Path]/lib"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${VLE_ABI_VERSION}.0;]/lib"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE ${VLE_ABI_VERSION}.0;]/lib"
        NO_DEFAULT_PATH)

  if (${_gvle_debug})
    message (" gvle_debug _gvle_base_include ${_gvle_base_include}")
    message (" gvle_debug _gvle_base_bin ${_gvle_base_bin}")
    message (" gvle_debug _gvle_base_lib ${_gvle_base_lib}")
  endif ()

  set(GVLE_INCLUDE_DIRS
    ${_gvle_base_include})

  set (GVLE_LIBRARY_DIRS
    ${_gvle_base_bin};${_gvle_base_lib})

  set (GVLE_LIBRARIES
    gvle-${VLE_ABI_VERSION} intl)
else () # find gvle using pkg-config
  find_package(PkgConfig REQUIRED)
  PKG_CHECK_MODULES(GVLE gvle-${VLE_ABI_VERSION})
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
