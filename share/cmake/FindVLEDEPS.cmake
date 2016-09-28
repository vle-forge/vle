# FindVLEDEPS.cmake
# =================
#
# Try to find VLE dependencies (libXml2, zlib, iconv, intl without)
# pkg-config (for windows install only)
#
# Copyright 2015-2016 INRA
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

include(FindPackageHandleStandardArgs)

#
# Try to found libxml2
#
find_path(LIBXML2_INCLUDE_DIR
  NAMES libxml/SAX2.h
  HINTS $ENV{VLEDEPS_BASEPATH}/include ${VLEDEPS_PATH}/include ${VLEDEPS_PATH}/include/libxml2
  PATH_SUFFIXES libxml2)

find_library(LIBXML2_LIBRARY
  NAMES xml2 libxml2
  HINTS $ENV{VLEDEPS_BASEPATH}/lib ${VLEDEPS_PATH}/lib)

find_package_handle_standard_args(LibXml2
  DEFAULT_MSG
  LIBXML2_LIBRARY
  LIBXML2_INCLUDE_DIR)

mark_as_advanced(LIBXML2_INCLUDE_DIR LIBXML2_LIBRARY)

#
# Try to found zlib
#
find_path(ZLIB_INCLUDE_DIR
  NAMES zlib.h
  HINTS $ENV{VLEDEPS_BASEPATH}/include ${VLEDEPS_PATH}/include)

find_library(ZLIB_LIBRARY
  NAMES z libz zlib
  HINTS $ENV{VLEDEPS_BASEPATH}/lib ${VLEDEPS_PATH}/lib)

find_package_handle_standard_args(zlib
  DEFAULT_MSG
  ZLIB_LIBRARY
  ZLIB_INCLUDE_DIR)

mark_as_advanced(ZLIB_INCLUDE_DIR ZLIB_LIBRARY)

#
# Try to found iconv
#
find_path(ICONV_INCLUDE_DIR
  NAMES iconv.h
  HINTS $ENV{VLEDEPS_BASEPATH}/include ${VLEDEPS_PATH}/include)

find_library(ICONV_LIBRARY
  NAMES iconv libiconv
  HINTS  $ENV{VLEDEPS_BASEPATH}/lib ${VLEDEPS_PATH}/lib)

find_package_handle_standard_args(iconv
  DEFAULT_MSG
  ICONV_LIBRARY
  ICONV_INCLUDE_DIR)

mark_as_advanced(ICONV_INCLUDE_DIR ICONV_LIBRARY)


if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  #
  # Try to found libintl
  #
  find_path(INTL_INCLUDE_DIR
    NAMES libintl.h
    HINTS $ENV{VLEDEPS_BASEPATH}/include ${VLEDEPS_PATH}/include)

  find_library(INTL_LIBRARY
    NAMES intl libintl
    HINTS $ENV{VLEDEPS_BASEPATH}/lib ${VLEDEPS_PATH}/lib)

  find_package_handle_standard_args(intl
    DEFAULT_MSG
    INTL_LIBRARY
    INTL_INCLUDE_DIR)
else ()
  set(ICONV_INCLUDE_DIR)
  set(ICONV_LIBRARY)
endif()

mark_as_advanced(ICONV_INCLUDE_DIR ICONV_LIBRARY)

#
# Build variable
#

list(APPEND VLEDEPS_INCLUDE_DIRS ${LIBXML2_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR}
  ${ICONV_INCLUDE_DIR} ${INTL_INCLUDE_DIR})

list(APPEND VLEDEPS_LIBRARIES ${LIBXML2_LIBRARY} ${ICONV_LIBRARY}
  ${INTL_LIBRARY} ${ICONV_LIBRARY} ${ZLIB_LIBRARY})

if (_vledeps_debug)
  message ("[FindVLEDEPS] libxml2 include path : ${LIBXML2_INCLUDE_DIR}")
  message ("              libxml2 libs : ${LIBXML2_LIBRARY}")
  message ("              zlib include path : ${ZLIB_INCLUDE_DIR}")
  message ("              zlib libs : ${ZLIB_LIBRARY}")
  message ("              iconv include path : ${ICONV_INCLUDE_DIR}")
  message ("              iconv libs : ${ICONV_LIBRARY}")
  message ("              intl include path : ${INTL_INCLUDE_DIR}")
  message ("              intl libs : ${INTL_LIBRARY}")
endif()

message ("VLEDEPS_LIBRARIES:" ${VLEDEPS_LIBRARIES})
message ("VLEDEPS_INCLUDE_DIRS:" ${VLEDEPS_INCLUDE_DIRS})

find_package_handle_standard_args(vledeps
  REQUIRED_VARS VLEDEPS_INCLUDE_DIRS VLEDEPS_LIBRARIES)
