# FindVLEDEPS.cmake
# =============
#
# Try to find VLE dependencies libarchive and libXml2 without pkg-config
# (for windows install only)
#
# Copyright 2015-2015 INRA
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
#  VLEDEPS_FOUND             - The system has VLE
#  VLEDEPS_INCLUDE_DIRS      - The VLE include directory
#  VLEDEPS_LIBRARIES         - Link these to use shared libraries of VLE
#
# CMake variables used by this module:
#
#  VLEDEPS_DEBUG             - If true, prints debug traces
#                            (default OFF)
#
# Environment variables used by this module:
#
#  VLEDEPS_BASEPATH          - environment variable for base path of vle
#
#=============================================================================
#
# Notes :
# ---------
#
#=============================================================================
#
# Usage
# -----
#
# set(VLEDEPS_DEBUG 1)
# find_package(VLEDEPS REQUIRED)
#
#=============================================================================

#
# Set default behavior of debug
#

if (DEFINED VLEDEPS_DEBUG)
  set (_vledeps_debug  ${VLEDEPS_DEBUG})
else ()
  set (_vledeps_debug 0)
endif ()

#
# Check Win32
#

if (NOT WIN32)
  message(FATAL_ERROR "FindVLEDEPS should be used only on win32")
endif ()

#
# Find LibArchive (home made to specify the path where it is located)
#
find_path(LibArchive_INCLUDE_DIR NAMES archive.h
   HINTS 
     $ENV{VLEDEPS_BASEPATH}/include
     ${VLEDEPS_PATH}/include
)
find_library(LibArchive_LIBRARIES NAMES archive libarchive
   HINTS 
     $ENV{VLEDEPS_BASEPATH}/lib
     ${VLEDEPS_PATH}/lib
)


#
# Find LibXml2 (home made since the distributed script uses pkg-config)
#
find_path(LibXml2_INCLUDE_DIR NAMES libxml/xpath.h
   HINTS 
     $ENV{VLEDEPS_BASEPATH}/include
     ${VLEDEPS_PATH}/include
   PATH_SUFFIXES libxml2
)
find_library(LibXml2_LIBRARIES NAMES xml2 libxml2
   HINTS 
     $ENV{VLEDEPS_BASEPATH}/lib
     ${VLEDEPS_PATH}/lib
)

if (_vledeps_debug)
  message ("[FindVLEDEPS] libarchive include path : ${LibArchive_INCLUDE_DIR}")
  message ("[FindVLEDEPS] libarchive libs : ${LibArchive_LIBRARIES}")
  message ("[FindVLEDEPS] libxml2 include path : ${LibXml2_INCLUDE_DIR}")
  message ("[FindVLEDEPS] libxml2 libs : ${LibXml2_LIBRARIES}")
endif()

set(VLEDEPS_INCLUDE_DIRS "${LibArchive_INCLUDE_DIR};${LibXml2_INCLUDE_DIR}")
set(VLEDEPS_LIBRARIES "${LibArchive_LIBRARIES};${LibXml2_LIBRARIES}")



# handle the QUIETLY and REQUIRED arguments and set VLE_FOUND to TRUE if all
# listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VLEDEPS REQUIRED_VARS VLEDEPS_INCLUDE_DIRS
                        VLEDEPS_LIBRARIES)

