# FindVLE.cmake
# =============
#
# Try to find VLE
#
# Copyright 2012-2018 INRA
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
#  VLE_VERSION           - gives the VLE version found (e.g 2.1)
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
#  VLE_VERSION           - environment variable with ABI version (2.0, 2.1 etc.)
#
#
# Example of use, find VLE with the minimal version of 2.0 :
#
# find_package(VLE 2.0 REQUIRED) 
#  
#=============================================================================

#
# Check VLE_VERSION
#

if (DEFINED ENV{VLE_VERSION})
  message(STATUS "FindVLE detects VLE ABI $ENV{VLE_VERSION}")
  set (_find_vle_all_versions $ENV{VLE_VERSION})
else ()
  message(STATUS "FindVLE searches VLE")
  set (_find_vle_all_versions 2.2 2.1 2.0)
endif ()

#
# Set default behavior of find vle
#

if (DEFINED VLE_DEBUG)
  set (_vle_debug  ${VLE_DEBUG})
else ()
  set (_vle_debug 0)
endif ()

if (WIN32)
  if (DEFINED VLE_USING_CMAKE)
     set (_find_vle_using_cmake ${VLE_USING_CMAKE})
  else (DEFINED VLE_USING_CMAKE)
     set (_find_vle_using_cmake 1)
  endif (DEFINED VLE_USING_CMAKE)
else (WIN32)
  find_package(PkgConfig REQUIRED)
  set (_find_vle_using_cmake 0)
endif (WIN32)

if (${_vle_debug})
  message ("[FindVLE] search VLE :")
  message ("[FindVLE]  - with minimal version "
           "${VLE_FIND_VERSION_MAJOR}.${VLE_FIND_VERSION_MINOR}")
  message ("[FindVLE]  - using internal cmake tools : ${_find_vle_using_cmake}")
endif ()

#
# Find VLE
#

foreach (_find_vle_test_version ${_find_vle_all_versions})
  if (${_vle_debug})
    message ("[FindVLE] search VLE ${_find_vle_test_version}")
  endif ()
  if (${_find_vle_using_cmake})
    find_path(_vle_base_include vle-${_find_vle_test_version}/vle/vle.hpp PATHS
      $ENV{VLE_BASEPATH}/include
      ${VLE_BASEPATH_LOCAL}/include
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${_find_vle_test_version};Path]/include"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${_find_vle_test_version};]/include"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE ${_find_vle_test_version};]/include"
      NO_DEFAULT_PATH)

    find_path(_vle_base_bin vle.exe PATHS
      $ENV{VLE_BASEPATH}/bin
      ${VLE_BASEPATH_LOCAL}/bin
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${_find_vle_test_version};Path]/bin"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${_find_vle_test_version};]/bin"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE ${_find_vle_test_version};]/bin"
      NO_DEFAULT_PATH)

    find_path(_vle_base_lib libvle-${_find_vle_test_version}.a PATHS
      $ENV{VLE_BASEPATH}/lib
      ${VLE_BASEPATH_LOCAL}/lib
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${_find_vle_test_version};Path]/lib"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE ${_find_vle_test_version};]/lib"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE ${_find_vle_test_version};]/lib"
      NO_DEFAULT_PATH)
 
    if (${_vle_debug})
      message ("[FindVLE] cmake _vle_base_include ${_vle_base_include}")
      message ("[FindVLE] cmake _vle_base_bin ${_vle_base_bin}")
      message ("[FindVLE] cmake _vle_base_lib ${_vle_base_lib}")
    endif ()

    if(_vle_base_include AND _vle_base_bin AND _vle_base_lib)
      set(VLE_INCLUDE_DIRS
        ${_vle_base_include}/vle-${_find_vle_test_version}; ${_vle_base_include};
        ${_vle_base_include}/libxml2)

      set(VLE_LIBRARY_DIRS
        ${_vle_base_bin};${_vle_base_lib})

      set(VLE_LIBRARIES
        vle-${_find_vle_test_version} xml2 intl)

      set (VLE_SHARE_DIR "${_vle_base_include}/../share/vle-${_find_vle_test_version}")

      set(VLE_VERSION ${_find_vle_test_version})

      break()
    endif ()
  else (${_find_vle_using_cmake})
    pkg_check_modules(_VLE-${_find_vle_test_version}
		vle-${_find_vle_test_version} QUIET)
    # select only the directory of vle, containing the pkgs directory
    # to build VLE_SHARE_DIR
    if (_VLE-${_find_vle_test_version}_FOUND)
      set(VLE_VERSION ${_find_vle_test_version})
      set(VLE_INCLUDE_DIRS ${_VLE-${_find_vle_test_version}_INCLUDE_DIRS})
      set(VLE_LIBRARY_DIRS ${_VLE-${_find_vle_test_version}_LIBRARY_DIRS})
      set(VLE_LIBRARIES ${_VLE-${_find_vle_test_version}_LIBRARIES})
      find_path(vle_lib_dir vle PATHS ${VLE_INCLUDE_DIRS} NO_DEFAULT_PATH)
      set(VLE_SHARE_DIR "${vle_lib_dir}/../../share/vle-${VLE_VERSION}")
      break ()
    endif ()
  endif (${_find_vle_using_cmake})
endforeach (_find_vle_test_version)

#error when VLE not found
if (NOT DEFINED VLE_VERSION)
  message (FATAL_ERROR "Missing VLE (set VLE_DEBUG to 1 for information)")
endif ()
#error when VLE too old
if (${VLE_VERSION} LESS "${VLE_FIND_VERSION_MAJOR}.${VLE_FIND_VERSION_MINOR}")
  message (FATAL_ERROR "VLE too old, version detected : ${VLE_VERSION}")
endif ()

# handle the QUIETLY and REQUIRED arguments and set VLE_FOUND to TRUE if all
# listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VLE REQUIRED_VARS VLE_INCLUDE_DIRS
                        VLE_LIBRARIES VLE_SHARE_DIR VLE_VERSION)

if (${_vle_debug})
  message ("[FindVLE] VLE_INCLUDE_DIRS ${VLE_INCLUDE_DIRS}")
  message ("[FindVLE] VLE_LIBRARY_DIRS ${VLE_LIBRARY_DIRS}")
  message ("[FindVLE] VLE_LIBRARIES ${VLE_LIBRARIES}")
  message ("[FindVLE] VLE_SHARE_DIR ${VLE_SHARE_DIR}")
  message ("[FindVLE] VLE_VERSION ${VLE_VERSION}")
endif ()

#mark_as_advanced(VLE_INCLUDE_DIRS VLE_LIBRARIES VLE_LIBRARY_DIRS)
