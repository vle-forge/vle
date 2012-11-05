# FindVLE.cmake
# =============

# Try to find VLE
#
# Copyright 2012 INRA
# Gauthier Quesnel <quesnel@users.sourceforge.net>
#
# Distributed under the OS-approved BSD License (the "License");
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#
# Once done this will define
#
#  VLE_FOUND - system has VLE
#  VLE_INCLUDE_DIR - the VLE include directory
#  VLE_LIBRARIES - Link these to use shared libraries of VLE
#  VLE_STATIC_LIBRARIES - Link these to use static libraries of VLE

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

if (NOT VLE_INCLUDE_DIR)
  find_path(VLE_INCLUDE_DIR vle/vle.hpp PATHS
    /usr/include
    /usr/local/include
    $ENV{HOME}/usr/include
    $ENV{VLE_BASEPATH}/include
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/include"
    PATH_SUFFIXES vle-1.1)
endif ()

if (NOT VLE_LIBRARIES)
  find_library(VLE_LIBRARIES
    NAMES vle-1.1 libvle-1.1
    HINTS $ENV{HOME}/usr $ENV{VLE_BASEPATH} 
    PATH_SUFFIXES lib lib64
    PATHS 
    /usr
    /usr/local 
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0\\;Path]/lib")
endif ()

if (NOT VLE_STATIC_LIBRARIES)
  find_library(VLE_STATIC_LIBRARIES
    NAMES vle-1.1.a libvle-1.1.a
    HINTS $ENV{HOME}/usr $ENV{VLE_BASEPATH} 
    PATH_SUFFIXES lib lib64
    PATHS 
    /usr
    /usr/local 
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0\\;Path]/lib")
endif ()

if (NOT VLE_STATIC_LIBRARIES OR NOT VLE_LIBRARIES OR NOT VLE_INCLUDE_DIR)
  message(STATUS "VLE include directories: ${VLE_INCLUDE_DIR}")
  message(STATUS "VLE libraries: ${VLE_LIBRARIES}")
  message(STATUS "VLE static libraries: ${VLE_STATIC_LIBRARIES}")
endif ()

# handle the QUIETLY and REQUIRED arguments and set VLE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VLE REQUIRED_VARS VLE_LIBRARIES
  VLE_STATIC_LIBRARIES VLE_INCLUDE_DIR)

mark_as_advanced(VLE_INCLUDE_DIR VLE_LIBRARIES VLE_STATIC_LIBRARIES)
