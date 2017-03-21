# FindVLE.cmake
# =============
#
# Try to find VLE
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
#  VLE_FOUND             - The system has VLE
#  VLE_INCLUDE_DIRS      - The VLE include directory
#  VLE_LIBRARY_DIRS      - Directories containing libraries to link
#  VLE_LIBRARIES         - Link these to use shared libraries of VLE
#  VLE_SHARE_DIR         - Base directory for VLE shared files
#
# CMake variables used by this module:
#  VLE_ABI_VERSION       - gives the VLE version to search for (e.g 1.1, 1.2)
#                          (REQUIRED)
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
# Usage
# -----
#
# set(VLE_ABI_VERSION 2.0)
# find_package(VLE REQUIRED)
#
#=============================================================================

#
# Check VLE_ABI_VERSION
#

if (NOT DEFINED VLE_ABI_VERSION)
  message (FATAL_ERROR "Cmake variable VLE_ABI_VERSION is not set")
endif ()


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

  find_path(_vle_base_include vle-${VLE_ABI_VERSION}/vle/vle.hpp PATHS
    $ENV{VLE_BASEPATH}/include
    ${VLE_BASEPATH_LOCAL}/include
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${VLE_ABI_VERSION}.0;Path]/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${VLE_ABI_VERSION}.0;]/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE ${VLE_ABI_VERSION}.0;]/include"
	NO_DEFAULT_PATH)

  find_path(_vle_base_bin vle.exe PATHS
    $ENV{VLE_BASEPATH}/bin
    ${VLE_BASEPATH_LOCAL}/bin
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${VLE_ABI_VERSION}.0;Path]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${VLE_ABI_VERSION}.0;]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE ${VLE_ABI_VERSION}.0;]/bin"
	NO_DEFAULT_PATH)

  find_path(_vle_base_lib libvle-${VLE_ABI_VERSION}.a PATHS
    $ENV{VLE_BASEPATH}/lib
    ${VLE_BASEPATH_LOCAL}/lib
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${VLE_ABI_VERSION}.0;Path]/lib"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${VLE_ABI_VERSION}.0;]/lib"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE ${VLE_ABI_VERSION}.0;]/lib"
	NO_DEFAULT_PATH)

  if (${_vle_debug})
    message (" vle_debug _vle_base_include ${_vle_base_include}")
    message (" vle_debug _vle_base_bin ${_vle_base_bin}")
    message (" vle_debug _vle_base_lib ${_vle_base_lib}")
  endif ()

  if(NOT _vle_base_include OR NOT _vle_base_bin OR NOT _vle_base_lib)
     message (FATAL_ERROR "Missing vle dependencies")
  endif ()

  set(VLE_INCLUDE_DIRS
    ${_vle_base_include}/vle-${VLE_ABI_VERSION}; ${_vle_base_include};
    ${_vle_base_include}/libxml2)

  set(VLE_LIBRARY_DIRS
    ${_vle_base_bin};${_vle_base_lib})

  set (VLE_LIBRARIES
    vle-${VLE_ABI_VERSION} xml2 intl)

  set (VLE_SHARE_DIR "${_vle_base_include}/../share/vle-${VLE_ABI_VERSION}")

else (${_find_vle_using_cmake})
  find_package(PkgConfig REQUIRED)
  PKG_CHECK_MODULES(VLE vle-${VLE_ABI_VERSION})
  # select only the directory of vle, containing the pkgs directory
  # to build VLE_SHARE_DIR
  find_path(vle_lib_dir vle PATHS
            ${VLE_INCLUDE_DIRS} NO_DEFAULT_PATH)
  set (VLE_SHARE_DIR "${vle_lib_dir}/../../share/vle-${VLE_ABI_VERSION}")
endif (${_find_vle_using_cmake})


# handle the QUIETLY and REQUIRED arguments and set VLE_FOUND to TRUE if all
# listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VLE REQUIRED_VARS VLE_INCLUDE_DIRS
                        VLE_LIBRARIES VLE_SHARE_DIR)

if (${_vle_debug})
  message (" vle_debug VLE_INCLUDE_DIRS ${VLE_INCLUDE_DIRS}")
  message (" vle_debug VLE_LIBRARY_DIRS ${VLE_LIBRARY_DIRS}")
  message (" vle_debug VLE_LIBRARIES ${VLE_LIBRARIES}")
  message (" vle_debug VLE_SHARE_DIR ${VLE_SHARE_DIR}")
endif ()

#mark_as_advanced(VLE_INCLUDE_DIRS VLE_LIBRARIES VLE_LIBRARY_DIRS)
