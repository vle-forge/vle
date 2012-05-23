cmake_minimum_required(VERSION 2.6)
project(VLE CXX C)
enable_testing()

set(VLE_MAJOR 1)
set(VLE_MINOR 1)
set(VLE_PATCH 0)
set(VLE_EXTRA "dev")
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
# CPack configuration
#
include(CMakeCPack.cmake)

#
# Go to the sources of the library. To control the type of the library
# (static or shared), use the CMake variable BUILD_SHARED_LIBS which
# is set to true by default.
#
option(BUILD_SHARED_LIBS "Build shared libraries." ON)

#
# CMake modules
#
include(CheckIncludeFileCXX)
include(CheckIncludeFile)
include(CheckLibraryExists)
include(CMakeDetermineCCompiler)

#
# append debug mode when compiler is GNU.
#
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif ()

#
# Check the curl library
#
find_package(CURL REQUIRED)

if (NOT CURL_FOUND)
  message(FATAL_ERROR "The curl library is required")
endif ()

#
# Check libraries with pkgconfig
#
find_package(PkgConfig REQUIRED)

# FIXME use old-style link directories for now
if (COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 OLD)
endif (COMMAND CMAKE_POLICY)

pkg_check_modules(GLIBMM REQUIRED glibmm-2.4)
pkg_check_modules(GTHREAD REQUIRED gthread-2.0)
pkg_check_modules(LIBXML REQUIRED libxml-2.0)
pkg_check_modules(CAIROMM cairomm-1.0>=1.2)
pkg_check_modules(GTKMM gtkmm-2.4)
pkg_check_modules(GTKSOURCEVIEWMM gtksourceviewmm-2.0)

option(WITH_CAIRO "use the cairomm to build output plugin [default: on]" ON)
if (WITH_CAIRO AND CAIROMM_FOUND)
    set(VLE_HAVE_CAIRO 1 CACHE INTERNAL "" FORCE)
else (WITH_CAIRO AND CAIROMM_FOUND)
    set(VLE_HAVE_CAIRO 0 CACHE INTERNAL "" FORCE)
endif (WITH_CAIRO AND CAIROMM_FOUND)

option(WITH_GVLE "use the gtkmm to build gvle [default: on]" ON)
if (WITH_GVLE AND GTKMM_FOUND)
  set(HAVE_GVLE 1 CACHE INTERNAL "" FORCE)
else (WITH_GVLE AND GTKMM_FOUND)
  set(HAVE_GVLE 0 CACHE INTERNAL "" FORCE)
endif (WITH_GVLE AND GTKMM_FOUND)

if (WITH_GVLE AND GTKSOURCEVIEWMM_FOUND)
   set(VLE_HAVE_GTKSOURCEVIEWMM 1 CACHE INTERNAL "" FORCE)
else (WITH_GVLE AND GTKSOURCEVIEWMM_FOUND)
   set(VLE_HAVE_GTKSOURCEVIEWMM 0 CACHE INTERNAL "" FORCE)
endif (WITH_GVLE AND GTKSOURCEVIEWMM_FOUND)

#
# Test the libboost header and libboost-text library.
#

option(WITH_TEST "use the libboost test unit library [default: on]" ON)
option(WITH_MPI "use an installed MPI library [default: on]" ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_NO_SYSTEM_PATHS OFF)
set(Boost_USE_STATIC_RUNTIME ON)
if (WIN32)
  set(Boost_THREADAPI "win32")
endif (WIN32)
set(Boost_DETAILED_FAILURE_MSG FALSE)
set(Boost_DEBUG FALSE)

find_package(Boost COMPONENTS unit_test_framework serialization filesystem
  system date_time regex thread program_options)
if (NOT Boost_SERIALIZATION_FOUND)
  message(FATAL_ERROR "The boost serialization library is required")
endif (NOT Boost_SERIALIZATION_FOUND)
if (NOT Boost_FILESYSTEM_FOUND)
  message(FATAL_ERROR "The boost filesystem library is required")
endif (NOT Boost_FILESYSTEM_FOUND)
if (NOT Boost_REGEX_FOUND)
  message(FATAL_ERROR "The boost regex library is required")
endif (NOT Boost_REGEX_FOUND)
if (NOT Boost_DATE_TIME_FOUND)
  message(FATAL_ERROR "The boost date_time library is required")
endif (NOT Boost_DATE_TIME_FOUND)
if (NOT Boost_THREAD_FOUND)
  message(FATAL_ERROR "The boost thread library is required")
endif (NOT Boost_THREAD_FOUND)
if (NOT Boost_PROGRAM_OPTIONS_FOUND)
  message(FATAL_ERROR "The boost program options library is required")
endif (NOT Boost_PROGRAM_OPTIONS_FOUND)
if (Boost_MINOR_VERSION AND ${Boost_MINOR_VERSION} LESS "35")
  set(Boost_SYSTEM_LIBRARY "")
endif (Boost_MINOR_VERSION AND ${Boost_MINOR_VERSION} LESS "35")
if (Boost_MINOR_VERSION AND ${Boost_MINOR_VERSION} LESS "41")
  set(VLE_HAVE_BOOST_SPIRIT2 0 CACHE INTERNAL "" FORCE)
else (Boost_MINOR_VERSION AND ${Boost_MINOR_VERSION} LESS "41")
  set(VLE_HAVE_BOOST_SPIRIT2 1 CACHE INTERNAL "" FORCE)
endif (Boost_MINOR_VERSION AND ${Boost_MINOR_VERSION} LESS "41")

if (WITH_TEST)
  if (Boost_UNIT_TEST_FRAMEWORK_FOUND)
    set(HAVE_UNITTESTFRAMEWORK 1 CACHE INTERNAL "" FORCE)
  endif (Boost_UNIT_TEST_FRAMEWORK_FOUND)
endif (WITH_TEST)

if (WITH_MPI)
  find_package(MPI REQUIRED)
  if (MPI_FOUND)
    message(STATUS "mpi include ${MPI_INCLUDE_PATH}")
    message(STATUS "mpi lib ${MPI_LIBRARY}")
    message(STATUS "mpi extra lib ${MPI_EXTRA_LIBRARY}")
    message(STATUS "mpi link ${MPI_LINK_FLAGS}")
    set(VLE_HAVE_MPI 1 CACHE INTERNAL "" FORCE)
  endif (MPI_FOUND)
endif (WITH_MPI)

#
# Generate the config.h
#
check_include_file_cxx(cxxabi.h VLE_HAVE_GCC_ABI_DEMANGLE)
check_include_file_cxx(execinfo.h VLE_HAVE_EXECINFO)
check_include_file_cxx(signal.h VLE_HAVE_SIGNAL)
include_directories(${VLE_BINARY_DIR})

#
# generate the vle.pc pkg-config file.
# Build some variable to build a correct Win32 pkg config file with:
# prefix=/target to detect the DIRNAME
# cflag=-Iboost_1_34_1 for the directory
#
if (UNIX)
  set(VLE_PKGCONFIG_PREFIXPATH "${CMAKE_INSTALL_PREFIX}")
  set(VLE_PKGCONFIG_BOOSTINCLUDE_DIRS "${Boost_INCLUDE_DIRS}")
  set(VLE_PKGCONFIG_LIBDIR "lib")
else (UNIX)
  set(VLE_PKGCONFIG_PREFIXPATH "/target")
  set(VLE_PKGCONFIG_BOOSTINCLUDE_DIRS "\${includedir}")
  set(VLE_PKGCONFIG_LIBDIR "bin")
endif (UNIX)

#
# Install the README
#
if (UNIX)
  install(FILES "README" DESTINATION "${VLE_SHARE_DIRS}")
  install(FILES "COPYING" DESTINATION "${VLE_SHARE_DIRS}")
else (UNIX)
  install(FILES "README" DESTINATION "${VLE_SHARE_DIRS}" RENAME "Readme.txt")
  install(FILES "COPYING" DESTINATION "${VLE_SHARE_DIRS}" RENAME "CopyRight.txt")
endif (UNIX)

#
# Define function
#
function(DeclareModuleDynamics name sources)
  add_library(${name} SHARED ${sources})

  target_link_libraries(${name}
    ${GLIBMM_LIBRARIES} ${LIBXML_LIBRARIES} ${GTHREAD_LIBRARIES} vleutils
    vlevalue vlegraph vlevpz vleoov vledevs vlemanager vletranslator)

  install(TARGETS ${name}
    RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/simulator"
    LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/simulator")
endfunction(DeclareModuleDynamics)

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
option(WITH_DOXYGEN "build the documentation with doxygen [default: off]" OFF)
if (WITH_DOXYGEN)
  find_package(Doxygen)
  if (DOXYGEN)
    add_subdirectory(doxygen)
  else (DOXYGEN)
    message(STATUS "WARNING: Doxygen not found - No reference manual create")
  endif (DOXYGEN)
endif (WITH_DOXYGEN)

#
# Browse the src subdirectory
#
ADD_SUBDIRECTORY(src)

#
# Status
#
message(STATUS "- - - -")
message(STATUS "${VLE_NAME_COMPLETE} configured successfully")
message(STATUS "Using ${CMAKE_INSTALL_PREFIX} for installation")
message(STATUS "Build type ${CMAKE_BUILD_TYPE}")
message(STATUS "Build with GCC ABI Demangle...: ${VLE_HAVE_GCC_ABI_DEMANGLE}")
message(STATUS "Build with execinfo.h.........: ${VLE_HAVE_EXECINFO}")
message(STATUS "Build unit test...............: ${HAVE_UNITTESTFRAMEWORK}")
message(STATUS "Build with cairo plugin.......: ${VLE_HAVE_CAIRO}")
message(STATUS "Build with gvle...............: ${HAVE_GVLE}")
message(STATUS "Build with gtksourceviewmm....: ${VLE_HAVE_GTKSOURCEVIEWMM}")
message(STATUS "Build with MPI................: ${VLE_HAVE_MPI}")
message(STATUS "Build shared library..........: ${BUILD_SHARED_LIBS}")

# vim:tw=0:ts=8:tw=0:sw=2:sts=2
