# FindGVLE_DEPS.cmake
# =============

# Try to find GVLE dependencies
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
#  GVLE_DEPS_FOUND - system has VLE
#  GVLE_DEPS_INCLUDE_DIRS - the VLE include directory
#  GVLE_DEPS_LIBRARY_DIRS - Directories containing libraries to link
#  GVLE_DEPS_LIBRARIES - Link these to use shared libraries of VLE
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
# find_package(GVLE_DEPS REQUIRED)
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
# Find GVLE_DEPS
#

if (${_find_vle_using_cmake})

  find_path(_base1 zlib.h PATHS
    $ENV{VLE_BASEPATH}/include
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/include")
  find_path(_base2 libz.a PATHS
    $ENV{VLE_BASEPATH}/lib
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/lib")


  if(NOT _base1
     OR NOT _base2)
     message (" debug _base1 ${_base1}")
     message (" debug _base2 ${_base2}")
     message (FATAL_ERROR "Missing gvle dependencies")
  endif ()


  set (GVLE_DEPS_INCLUDE_DIRS
       ${_base1}/gtksourceview-2.0;
       ${_base1}/gtksourceviewmm-2.0;${_base2}/gtksourceviewmm-2.0/include;
       ${_base1}/gtkmm-2.4;${_base2}/gtkmm-2.4/include;
       ${_base1}/cairomm-1.0;${_base2}/cairomm-1.0/include;
       ${_base1}/atkmm-1.6;
       ${_base1}/giomm-2.4;${_base2}/giomm-2.4/include;
       ${_base1}/pangomm-1.4;${_base2}/pangomm-1.4/include;
       ${_base1}/gtk-2.0;${_base1}/gtk-unix-print-2.0;
       ${_base1}/gdkmm-2.4;${_base2}/gdkmm-2.4/include;
       ${_base1}/atk-1.0;
       ${_base1}/pango-1.0;
       ${_base1}/cairo;
       ${_base1}/pixman-1;
       ${_base1}/freetype2;
       ${_base1}/libpng12;
       ${_base2}/gtk-2.0/include;
       ${_base1}/gdk-pixbuf-2.0;
       ${_base1}/gio-unix-2.0)
  set (GVLE_DEPS_LIBRARY_DIRS "")
  set (GVLE_DEPS_LIBRARIES
       ${_base2}/libgtksourceview-2.0.a;${_base2}/libgtksourceviewmm-2.0.a;
       ${_base2}/libgtkmm-2.4.a;
       ${_base2}/libatkmm-1.6.a;${_base2}/libgdkmm-2.4.a;
       ${_base2}/libgiomm-2.4.a;${_base2}/libpangomm-1.4.a;
       ${_base2}/libgtk-win32-2.0.a
       ${_base2}/liblzma.a;
       ${_base2}/libws2_32.a;
       #${_base2}/libshlwapi.a;
       ${_base2}/libpcre.a;
       ${_base2}/libwinspool.a;
       ${_base2}/libcomctl32.a;
       ${_base2}/libcomdlg32.a;
       ${_base2}/libcairomm-1.0.a;
       ${_base2}/libgdk-win32-2.0.a;
       ${_base2}/libimm32.a;
       ${_base2}/libshell32.a;${_base2}/libole32.a;${_base2}/libuuid.a; #system
       ${_base2}/libatk-1.0.a;
       ${_base2}/libpangocairo-1.0.a;
       ${_base2}/libgio-2.0.a;
       ${_base2}/libshlwapi.a;#before gio
       ${_base2}/libdnsapi.a;
       ${_base2}/libgdk_pixbuf-2.0.a;
       ${_base2}/libtiff.a;
       ${_base2}/libjpeg.a;
       ${_base2}/libpangoft2-1.0.a;
       ${_base2}/libpangowin32-1.0.a;
       ${_base2}/libfontconfig.a;
       ${_base2}/libexpat.a;
       #${_base2}/libfreetype.a;
       ${_base2}/libpango-1.0.a;
       ${_base2}/libm.a;
       ${_base2}/libusp10.a;
       ${_base2}/libpng15.a;
       ${_base2}/libgmodule-2.0.a;
       ${_base2}/libcairo.a;
       ${_base2}/libfreetype.a;#before cairo
       ${_base2}/libz.a;
       ${_base2}/libmsimg32.a;
       ${_base2}/libgdi32.a;
       ${_base2}/libpixman-1.a)

else (${_find_vle_using_cmake})

  include(FindPackageHandleStandardArgs)
  find_package(PkgConfig REQUIRED)
  PKG_CHECK_MODULES(GTKSOURCEVIEWMM gtksourceviewmm-2.0)
  if (${GTKSOURCEVIEWMM_FOUND})
    set (GVLE_DEPS_INCLUDE_DIRS ${GTKSOURCEVIEWMM_INCLUDE_DIRS})
    set (GVLE_DEPS_LIBRARY_DIRS ${GTKSOURCEVIEWMM_LIBRARY_DIRS})
    set (GVLE_DEPS_LIBRARIES ${GTKSOURCEVIEWMM_LIBRARIES})
    set (GVLE_DEPS_FOUND TRUE)
  else (${GTKSOURCEVIEWMM_FOUND})
    set (GVLE_DEPS_INCLUDE_DIRS "")
    set (GVLE_DEPS_LIBRARY_DIRS "")
    set (GVLE_DEPS_LIBRARIES "")
    set (GVLE_DEPS_FOUND TRUE)
  endif (${GTKSOURCEVIEWMM_FOUND})

endif (${_find_vle_using_cmake})


message (" Ronan ${GVLE_DEPS_LIBRARIES} ")
mark_as_advanced(GVLE_DEPS_INCLUDE_DIRS GVLE_DEPS_LIBRARY_DIRS
               GVLE_DEPS_LIBRARIES)
