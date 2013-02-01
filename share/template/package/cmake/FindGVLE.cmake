# FindGVLE.cmake
# =============

# Try to find GVLE
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
#  GVLE_FOUND - system has VLE
#  GVLE_INCLUDE_DIR - the VLE include directory
#  GVLE_LIBRARY_DIRS - Directories containing libraries to link
#  GVLE_LIBRARIES - Link these to use shared libraries of VLE
#
# Options:
#  
#  FIND_VLE_USING_CMAKE - If true, on windows, use cmake for finding GVLE, 
#                         else use pkgconfig 
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

#
# Set default behavior of find vle
#

if (WIN32)
  if (DEFINED FIND_VLE_USING_CMAKE)
     set (_find_vle_using_cmake ${FIND_VLE_USING_CMAKE})
  else (DEFINED FIND_VLE_USING_CMAKE)
     set (_find_vle_using_cmake 1)
  endif (DEFINED FIND_VLE_USING_CMAKE)
else (WIN32)
  set (_find_vle_using_cmake 0)
endif (WIN32)

#
# Find GVLE
#

if (${_find_vle_using_cmake})

  message ( " --- cmake " )
  if (NOT GVLE_INCLUDE_DIR)
    find_path(GVLE_INCLUDE_DIR vle/gvle/GVLE.hpp PATHS
      /usr/include
      /usr/local/include
      $ENV{HOME}/usr/include
      $ENV{VLE_BASEPATH}/include
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/include"
      PATH_SUFFIXES vle-1.1)
  endif ()

  if (NOT GVLE_LIBRARIES)
    find_library(GVLE_LIBRARIES
      NAMES libgvle-1.1
      HINTS $ENV{HOME}/usr $ENV{VLE_BASEPATH} 
      PATH_SUFFIXES lib lib64
      PATHS 
      /usr
      /usr/local 
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0\\;Path]/lib")
  endif ()

#if (NOT GVLE_LIBRARIES OR NOT GVLE_INCLUDE_DIR)
  message(STATUS "GVLE include directories: ${GVLE_INCLUDE_DIR}")
  message(STATUS "GVLE libraries: ${GVLE_LIBRARIES}")
#endif ()

  # handle the QUIETLY and REQUIRED arguments and set VLE_FOUND to TRUE if
  # all listed variables are TRUE
  include(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(GVLE REQUIRED_VARS GVLE_LIBRARIES
    GVLE_INCLUDE_DIR)

  set (GVLE_LIBRARY_DIRS "")
  message (" Ronan ${GVLE_LIBRARIES} ") 

else (${_find_vle_using_cmake})

  message ( " --- pkg " )

  include(FindPackageHandleStandardArgs)
  find_package(PkgConfig REQUIRED)

  PKG_CHECK_MODULES(GVLE gvle-1.1 REQUIRED)

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(GVLE REQUIRED_VARS
    GVLE_LIBRARIES
    GVLE_LIBRARY_DIRS
    GVLE_INCLUDE_DIRS)

endif (${_find_vle_using_cmake})

mark_as_advanced(GVLE_INCLUDE_DIR GVLE_LIBRARIES GVLE_LIBRARY_DIRS)