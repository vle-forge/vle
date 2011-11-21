# VleCheckPackageConfig.cmake
# ===========================
#
# Check the dependencies of VLE's packages
#
# Copyright (c) 2011 INRA
# Gauthier Quesnel <quesnel@users.sourceforge.net>
# 
# Distributed under the OS-approved BSD License (the "License");
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#
# ===========================================================================
#
# Changelog
# ---------
#
# 1 Initial version.
#
# ===========================================================================
#
# Usage
# -----
#
# FIND_PACKAGE(VleCheckPackage REQUIRED)
# VLE_CHECK_PACKAGE(PREFIX package)
#
# The script sets the following variables:
#   VLE_CHECK_PACKAGE_VERSION ... the version of this script (integer)
#   <PREFIX>_FOUND            ... set to 1 is package exist
#   <PREFIX>_LIBRARIES        ... only the libraries (w/o the '-l') found into
#                                 the lib directory of the package.
#  <PREFIX>_INCLUDE_DIRS      ... the '-I' preprocessor flags (w/o the '-I')
#
#
# Example
# -------
#
# FIND_PACKAGE(VleCheckPackage REQUIRED)
# VLE_CHECK_PACKAGE(DECISION decision-ext-1.0)
#
# IF (NOT DECISION_FOUND)
#   message(SEND_ERROR "Decision extension not found")
# ENDIF (NOT DECISION_FOUND)
#
# ADD_LIBRARY(simple SHARED simple.cpp)
# INCLUDE_DIRECTORIES(${VLE_INCLUDE_DIRS} ${DECISION_INCLUDE_DIRS})
# TARGET_LINK_LIBRARIES(simple ${VLE_LIBRARIES} ${DECISION_LIBRARIES})
#
# ===========================================================================

set(VLE_CHECK_PACKAGE_VERSION 1)

macro(_vle_check_package_set var value)
  set(${var} "${value}" CACHE INTERNAL "")
endmacro(_vle_check_package_set)

macro(_vle_check_package_unset var)
  set(${var} "" CACHE INTERNAL "")
endmacro(_vle_check_package_unset)


##
## user visible macros start here
##

macro(vle_check_package _prefix _module)
  file(TO_CMAKE_PATH "$ENV{VLE_HOME}" _vle_home)
  MESSAGE(STATUS "vle home: [${_vle_home}]")

  if (NOT _vle_home)
    message(STATUS "The VLE_HOME directory not assigned try default")
    if (CMAKE_HOST_WIN32)
      file(TO_CMAKE_PATH "$ENV{HOMEDRIVE}$ENV{HOMEPATH}/vle" _vle_home)
    else (CMAKE_HOST_WIN32)
      file(TO_CMAKE_PATH "$ENV{HOME}/.vle" _vle_home)
    endif (CMAKE_HOST_WIN32)
  endif (NOT _vle_home)

  if (EXISTS ${_vle_home})
    set(_vle_include_dirs "${_vle_home}/pkgs/${_module}/src")
    set(_vle_lib_dirs "${_vle_home}/pkgs/${_module}/lib")

    if (EXISTS "${_vle_include_dirs}" AND EXISTS "${_vle_lib_dirs}")

      if (CMAKE_HOME_WIN32)
        file(GLOB _vle_libraries "${_vle_lib_dirs}/*.dll")
      else (CMAKE_HOME_WIN32)
        file(GLOB _vle_libraries "${_vle_lib_dirs}/*.so")
      endif (CMAKE_HOME_WIN32)

      _vle_check_package_set(${_prefix}_FOUND 1)
      _vle_check_package_set(${_prefix}_INCLUDE_DIRS ${_vle_include_dirs})
      _vle_check_package_set(${_prefix}_LIBRARIES ${_vle_libraries})
    else (EXISTS "${_vle_include_dirs}" AND EXISTS "${_vle_lib_dirs}")
      message(SEND_ERROR "Package `${_module}' not found")
    endif (EXISTS "${_vle_include_dirs}" AND EXISTS "${_vle_lib_dirs}")
  else (EXISTS ${_vle_home})
    message(SEND_ERROR "Environment variable VLE_HOME not found")
  endif (EXISTS ${_vle_home})

endmacro(vle_check_package)
