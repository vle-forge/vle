# FindVLE_DEPS.cmake
# =============

# Try to find VLE dependencies
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
#  VLE_DEPS_FOUND - system has VLE
#  VLE_DEPS_INCLUDE_DIRS - the VLE include directory
#  VLE_DEPS_LIBRARY_DIRS - Directories containing libraries to link
#  VLE_DEPS_LIBRARIES - Link these to use shared libraries of VLE
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
# find_package(VLE_DEPS REQUIRED)
#
#=============================================================================

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
# Find VLE_DEPS
#

if (${_find_vle_using_cmake})

  find_path(_base1 zlib.h PATHS
    $ENV{VLE_BASEPATH}/include
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/include")
  find_path(_base2 libz.a PATHS
    $ENV{VLE_BASEPATH}/lib
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/lib")

  if(NOT _base1 OR NOT _base2)
     message (" debug _base1 ${_base1}")
     message (" debug _base2 ${_base2}")
     message (FATAL_ERROR "Missing vle dependencies")
  endif ()

  set (VLE_DEPS_INCLUDE_DIRS ${_base1};${_base1}/libxml2;
       ${_base1}/glibmm-2.4;${_base2}/glibmm-2.4/include;
       ${_base1}/sigc++-2.0;${_base2}/sigc++-2.0/include;
       ${_base1}/glib-2.0;${_base2}/glib-2.0/include)
  set (VLE_DEPS_LIBRARY_DIRS "")
  set (VLE_DEPS_LIBRARIES ${_base2}/libxml2.a;${_base2}/libglibmm-2.4.a;
       ${_base2}/libgobject-2.0.a;${_base2}/libsigc-2.0.a;
       ${_base2}/libgthread-2.0.a;${_base2}/libglib-2.0.a;
       ${_base2}/libintl.a;${_base2}/libiconv.a;${_base2}/libpcre.a;
       ${_base2}/libws2_32.a;${_base2}/liblzma.a)

else (${_find_vle_using_cmake})

  set (VLE_DEPS_INCLUDE_DIRS "")
  set (VLE_DEPS_LIBRARY_DIRS "")
  set (VLE_DEPS_LIBRARIES "")
  set (VLE_DEPS_FOUND TRUE)
  mark_as_advanced(VLE_DEPS_INCLUDE_DIRS VLE_DEPS_LIBRARIES 
    VLE_DEPS_LIBRARY_DIRS)

endif (${_find_vle_using_cmake})

mark_as_advanced(VLE_DEPS_INCLUDE_DIRS VLE_DEPS_LIBRARY_DIRS
               VLE_DEPS_LIBRARIES)
