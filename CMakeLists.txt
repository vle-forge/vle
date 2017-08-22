cmake_minimum_required(VERSION 2.8)
project(VLE CXX C)
enable_testing()

#
# Options
#

option(WITH_FULL_OPTIMIZATION "disable all logging facilities and active heavy optimization code to speed up simulation. [default: off]" OFF)
option(WITH_DEBUG "enable debug log message. It slows simulation [default: ON]" ON)
option(WITH_GVLE "use QT to build gvle [default: on]" ON)
option(WITH_TEST "build unit test [default: on]" ON)
option(WITH_DOXYGEN "build the documentation with doxygen [default: off]" OFF)
option(WITH_MVLE "build mvle [default: off]" OFF)
option(WITH_CVLE "build cvle [default: off]" OFF)
option(WITH_WIN32_INSTALLER "prepare win32 installer [default: off]" OFF)

#
# Usefull variables
#

set(VLE_MAJOR 2)
set(VLE_MINOR 0)
set(VLE_PATCH 0)
set(VLE_EXTRA "alpha2")
set(VLE_VERSION "${VLE_MAJOR}.${VLE_MINOR}.${VLE_PATCH}")
set(VLE_VERSION_SHORT "${VLE_MAJOR}.${VLE_MINOR}")
set(VLE_NAME "vle")
set(VLE_LOCALE_NAME "${VLE_NAME}-${VLE_MAJOR}-${VLE_MINOR}")

if ("${VLE_EXTRA}" STREQUAL "")
  set(VLE_COMPLETE "${VLE_MAJOR}.${VLE_MINOR}.${VLE_PATCH}")
else ("${VLE_EXTRA}" STREQUAL "")
  set(VLE_COMPLETE "${VLE_MAJOR}.${VLE_MINOR}.${VLE_PATCH}-${VLE_EXTRA}")
endif ("${VLE_EXTRA}" STREQUAL "")

set(VLE_NAME_COMPLETE "${VLE_NAME}-${VLE_COMPLETE}")

#
# Installation variables
#

set(VLE_LIBRARY_DIRS "lib/vle-${VLE_VERSION_SHORT}")
set(VLE_INCLUDE_DIRS "include/vle-${VLE_VERSION_SHORT}/vle")
set(VLE_INCLUDE_DIR "include/vle-${VLE_VERSION_SHORT}")
set(VLE_MANPAGE_PATH "share/man/man1")
set(VLE_SHARE_DIRS "share/vle-${VLE_VERSION_SHORT}")


#
# CMake modules
#
include(CheckIncludeFileCXX)
include(CheckIncludeFile)
include(CheckLibraryExists)
include(CMakeDetermineCCompiler)
include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
CHECK_CXX_COMPILER_FLAG("-std=c++1y" COMPILER_SUPPORTS_CXX1Y)

if (COMPILER_SUPPORTS_CXX14)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif(COMPILER_SUPPORTS_CXX1Y)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14/1y support."
    "Please use a different C++ compiler.")
endif()

#
# append debug mode when compiler is GNU.
#

if (CMAKE_BUILD_TYPE STREQUAL "")
  SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
endif ()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

  option(WITH_COVERAGE "use coverage build [default: off]" OFF)
  if (WITH_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --coverage")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
  endif ()

  if ("${CMAKE_BUILD_TYPE}" EQUAL "Debug" OR
      "${CMAKE_BUILD_TYPE}" EQUAL "RelWithDebInfo")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3")
  endif ()
endif ()

# FIXME use old-style link directories for now
if (COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 OLD)
  cmake_policy(SET CMP0020 NEW)#for qt link
endif (COMMAND CMAKE_POLICY)

#
# Detect thread library to use
#
find_package(Threads REQUIRED)

set(QMAKE_QUERY_LIBPATH qmake -query QT_INSTALL_LIBS)
execute_process(COMMAND ${QMAKE_QUERY_LIBPATH} OUTPUT_VARIABLE
  CMAKE_PREFIX_PATH)
set(QMAKE_QUERY_HOSTPATH qmake -query QT_HOST_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${QMAKE_QUERY_HOSTPATH} OUTPUT_VARIABLE
  QT_INSTALL_PATH)

if (WITH_GVLE)
  find_package(Qt5 COMPONENTS Widgets Xml Gui Help PrintSupport)
  if (NOT Qt5_FOUND)
    message(FATAL_ERROR "Qt5 is required")
  endif (NOT Qt5_FOUND)
  if (NOT Qt5Widgets_FOUND)
    message(FATAL_ERROR "Qt5Widgets is required")
  endif (NOT Qt5Widgets_FOUND)
  if (NOT Qt5Xml_FOUND)
    message(FATAL_ERROR "Qt5Xml is required")
  endif (NOT Qt5Xml_FOUND)
  if (NOT Qt5Gui_FOUND)
    message(FATAL_ERROR "Qt5Gui is required")
  endif (NOT Qt5Gui_FOUND)
  if (NOT Qt5Help_FOUND)
    message(FATAL_ERROR "Qt5Help is required")
  endif (NOT Qt5Help_FOUND)
  if (NOT Qt5PrintSupport_FOUND)
    message(FATAL_ERROR "Qt5PrintSupport is required")
  endif (NOT Qt5PrintSupport_FOUND)
  set(VLE_HAVE_QT 1 CACHE INTERNAL "" FORCE)
  set(VLE_HAVE_GVLE 1 CACHE INTERNAL "" FORCE)
endif ()


message (" dbg Root ${Qt5Widgets_INCLUDES} ${Qt5Widgets_INCLUDE_DIRS}")

if (WIN32)
  SET (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/share/cmake ${CMAKE_MODULE_PATH})
  set(VLEDEPS_DEBUG 1)
  find_package(VLEDEPS REQUIRED)
else ()
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(VLEDEPS libxml-2.0)
  if (NOT VLEDEPS_FOUND)
    message(FATAL_ERROR "Failed to find minimal dependencies: libxml-2.0")
  endif ()
endif ()

# Propagates -std=C++11 CC option from pkg-config but VLEDEPS_CFLAGS_OTHER
# #seems to be a CMake'list. So we replace ';' with ' '.
string(REPLACE ";" " " VLEDEPS_CFLAGS_OTHER_STRING "${VLEDEPS_CFLAGS_OTHER}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VLEDEPS_CFLAGS_OTHER_STRING}")

#
# Test the libboost header and libboost-text library.
#

# set(Boost_USE_STATIC_LIBS OFF)
# set(Boost_USE_STATIC_RUNTIME OFF)

set(Boost_USE_MULTITHREADED ON)
set(Boost_DETAILED_FAILURE_MSG ON)
set(Boost_DEBUG OFF)

if (VLE_BOOST_INCLUDE_PATH AND VLE_BOOST_LIBRARIES_PATH)
 set (BOOST_LIBRARYDIR ${VLE_BOOST_LIBRARIES_PATH})
 set (BOOST_INCLUDEDIR ${VLE_BOOST_INCLUDE_PATH})
endif ()

if (WITH_MVLE OR WITH_CVLE)
  find_package(Boost COMPONENTS unit_test_framework serialization mpi)

  if (NOT Boost_SERIALIZATION_FOUND)
    message(STATUS "The boost serialization was not found, MPI disabled")
  endif ()
  if (NOT Boost_MPI_FOUND)
    message(STATUS "The boost mpi was not found, MPI disabled")
  endif ()
else ()
  find_package(Boost REQUIRED)
endif ()

if (WITH_TEST)
  set(VLE_HAVE_UNITTESTFRAMEWORK 1 CACHE INTERNAL "" FORCE)
endif (WITH_TEST)

#
# Check for an MPI implementation. building mvle and/or cvle
#

if (WITH_MVLE OR WITH_CVLE)
  find_package(MPI REQUIRED)
  message(STATUS "mpi include ${MPI_INCLUDE_PATH}")
  message(STATUS "mpi lib ${MPI_LIBRARY}")
  message(STATUS "mpi extra lib ${MPI_EXTRA_LIBRARY}")
  message(STATUS "mpi link ${MPI_LINK_FLAGS}")
  if (WITH_MVLE)
    set(VLE_HAVE_MVLE 1 CACHE INTERNAL "" FORCE)
  else ()
    set(VLE_HAVE_MVLE 0 CACHE INTERNAL "" FORCE)
  endif ()
  if (WITH_CVLE)
    if (Boost_SERIALIZATION_FOUND AND Boost_MPI_FOUND)
      set(VLE_HAVE_CVLE 1 CACHE INTERNAL "" FORCE)
    else ()
      message(WARNING "boost mpi or boost serialization not found to build cvle")
      set(VLE_HAVE_CVLE 0 CACHE INTERNAL "" FORCE)
    endif ()
  else ()
    set(VLE_HAVE_CVLE 0 CACHE INTERNAL "" FORCE)
  endif ()
else ()
  set(VLE_HAVE_MVLE 0 CACHE INTERNAL "" FORCE)
  set(VLE_HAVE_CVLE 0 CACHE INTERNAL "" FORCE)
endif ()


include_directories(${VLE_BINARY_DIR})

#
# generate the vle.pc pkg-config file.
# Build some variable to build a correct Win32 pkg config file with:
# prefix=/target to detect the DIRNAME
# cflag=-Iboost_1_34_1 for the directory
#

set(VLE_PKGCONFIG_CFLAGS_OTHER "${VLEDEPS_CFLAGS_OTHER_STRING}")
if (UNIX)
  set(VLE_PKGCONFIG_PREFIXPATH "${CMAKE_INSTALL_PREFIX}")
  set(VLE_PKGCONFIG_BOOSTINCLUDE_DIRS "${Boost_INCLUDE_DIRS}")
  set(VLE_PKGCONFIG_LIBDIR "lib")
else ()
  set(VLE_PKGCONFIG_PREFIXPATH "/target")
  set(VLE_PKGCONFIG_BOOSTINCLUDE_DIRS "\${includedir}")
  set(VLE_PKGCONFIG_LIBDIR "bin")
endif ()

#
# Install the README
#
if (UNIX)
  install(FILES "README.md" DESTINATION "${VLE_SHARE_DIRS}")
  install(FILES "COPYING" DESTINATION "${VLE_SHARE_DIRS}")
else ()
  install(FILES "README.md" DESTINATION "${VLE_SHARE_DIRS}" RENAME "Readme.txt")
  install(FILES "COPYING" DESTINATION "${VLE_SHARE_DIRS}" RENAME "CopyRight.txt")
endif ()

#
# Add sources for a target
# add_sources(<target> <source1> [<source2> ...])
#
function(add_sources target)
  get_property(prop_defined GLOBAL PROPERTY ${target}_SRCS DEFINED)
  if(NOT prop_defined)
    define_property(GLOBAL PROPERTY ${target}_SRCS
      BRIEF_DOCS "Sources for the ${target} target"
      FULL_DOCS "List of source files for the ${target} target")
  endif()

  set(SRCS)
  foreach(src ${ARGN})
    if(NOT IS_ABSOLUTE "${src}")
      get_filename_component(src "${src}" ABSOLUTE)
    endif()
    list(APPEND SRCS "${src}")
  endforeach()
  set_property(GLOBAL APPEND PROPERTY "${target}_SRCS" "${SRCS}")
endfunction()

#
# Browse the share subdirectory
#
add_subdirectory(share)

#
# Gettext utilities
#
find_package(Gettext)
if (GETTEXT_FOUND)
  set(VLE_HAVE_NLS 1 CACHE INTERNAL "" FORCE)
  add_subdirectory(i18n)
else (GETTEXT_FOUND)
  set(VLE_HAVE_NLS 0 CACHE INTERNAL "" FORCE)
endif (GETTEXT_FOUND)

#
# Generating documentation with doxygen
#

if (WITH_DOXYGEN)
  find_package(Doxygen)
  if (DOXYGEN)
    add_subdirectory(doxygen)
  else (DOXYGEN)
    message(STATUS "WARNING: Doxygen not found - No reference manual create")
  endif (DOXYGEN)
endif (WITH_DOXYGEN)

#
# Build a generic make uninstall command
#

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

#
# Browse the src subdirectory
#
add_subdirectory(src)

#
# Status
#
message(STATUS "- - - -")
message(STATUS "${VLE_NAME_COMPLETE} configured successfully")
message(STATUS "Using ${CMAKE_INSTALL_PREFIX} for installation")
message(STATUS "Build type ${CMAKE_BUILD_TYPE}")
message(STATUS "Full optimization.............: ${WITH_FULL_OPTIMIZATION}")
message(STATUS "Show debug message............. ${WITH_DEBUG}")
message(STATUS "Build unit test...............: ${VLE_HAVE_UNITTESTFRAMEWORK}")
message(STATUS "Build with gvle...............: ${VLE_HAVE_GVLE}")
message(STATUS "Build with mvle...............: ${VLE_HAVE_MVLE}")
message(STATUS "Build with cvle...............: ${VLE_HAVE_CVLE}")

# vim:tw=0:ts=8:tw=0:sw=2:sts=2

include(CMakeCPack.cmake)
