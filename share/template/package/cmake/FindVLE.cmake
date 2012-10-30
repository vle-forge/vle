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
			$ENV{VLE_BASEPATH}/include
			"[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/include"
			PATH_SUFFIXES vle-1.1)
endif ()

if (NOT VLE_LIBRARIES)
    find_library(VLE_LIBRARIES NAMES vle-1.1
			/usr/lib
			/usr/lib64
			/usr/local/lib
			/usr/local/lib64
			$ENV{VLE_BASEPATH}/lib
			"[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0\\;Path]/lib"
			PATH_SUFFIXES lib)
endif ()

if (NOT VLE_STATIC_LIBRARIES)
	find_library(VLE_STATIC_LIBRARIES NAMES vle-1.1.a
			/usr/lib
			/usr/lib64
			/usr/local/lib
			/usr/local/lib64
			$ENV{VLE_BASEPATH}/lib
			"[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0\\;Path]/lib"
			PATH_SUFFIXES lib)
endif ()

# handle the QUIETLY and REQUIRED arguments and set VLE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VLE DEFAULT_MSG
								  REQUIRED_VARS VLE_LIBRARIES
								  VLE_STATIC_LIBRARIES VLE_INCLUDE_DIR)

mark_as_advanced(VLE_INCLUDE_DIR VLE_LIBRARIES VLE_STATIC_LIBRARIES)
