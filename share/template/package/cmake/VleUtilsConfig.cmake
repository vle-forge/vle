# VleCheckDependencies.cmake
# ===========================
#
# Check the dependencies of the current package
#
# Copyright (c) 2014-2014 INRA
# Ronan Trépos <rtrepos@toulouse.inra.fr>
#
# Distributed under the OS-approved BSD License (the "License");
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#
# ===========================================================================
#
# Changelog
# ---------
#
# 1.0  Initial version.
#
# ===========================================================================
#
# Description
# -----------
#
# Following CMake variables have to be defined :
#  
#  VLE_DEBUG       - if true, prints debug traces (default OFF)
#  VLE_ABI_VERSION - it gives the ABI version of installed vle. For example,
#                    if VLE_ABI_VERSION = "1.2", then packages are expected to
#                    be installed into VLE_HOME/pkgs-1.2.
#
# This script provides utils macros for vle cmake files :
#
#####
#
# VleCheckPackage: it checks if a package is installed into the binary 
#     packages. And sets the variables used to link to the designated package.
#
#> set(VLE_ABI_VERSION 1.2)
#> find_package(VleUtils REQUIRED)
#> VleCheckPackage(MYPKG mypackage)
#> VleCheckPackage(MYPKG2 mypackage2(>=1.0))
#
# The script sets the following variables:
#   <MYPKG>_FOUND          ... set to 1 if package exists and the version id
#                              matches the requirement.
#   <MYPKG>_LIBRARIES      ... only the libraries (w/o the '-l') found into
#                                 the lib directory of the package.
#   <MYPKG>_INCLUDE_DIRS   ... the '-I' preprocessor flags (w/o the '-I').
#
#####
#
# VleCheckDependencies: it checks if vle dependencies are installed into the
#     binary packages (VLE_HOME/pkgs-${VLE_ABI_VERSION}). Dependencies are
#     declared into the flag "Build-Depends" of the file Description.txt
#     (${CMAKE_SRC_DIR}/Description.txt).
#
#> set(VLE_ABI_VERSION 1.2)
#> find_package(VleUtils REQUIRED)
#> VleCheckDependencies()
#
#####
#
# VleBuildDynamic: it builds a vle dynamic from a cpp file taking into account
#     packages dependencies declared into tag "@@tagdepends" into the cpp file
#
#> set(VLE_ABI_VERSION 1.2)
#> find_package(VleUtils REQUIRED)
#> VleBuildDynamic(Simple "Simple.cpp;otherSouce.cpp")
#
# The Simple.cpp can contain the 'tagdepends', of the form  
#
#// @@tagdepends: pkg1,pkg2 @@endtagdepends
#
#####
#
# VleBuildTest: it builds test for the current project, from a cpp file
#
#> set(VLE_ABI_VERSION 1.2)
#> find_package(VleUtils REQUIRED)
#> VleBuildTest(test_pkg "test.cpp")
#
#####
#
# VleBuildOovPlugin: it builds a oov plugin from a cpp file
#
#> set(VLE_ABI_VERSION 1.2)
#> find_package(VleUtils REQUIRED)
#> VleBuildOovPlugin(oovplug_name "source1.cpp;source2.cpp")
#
#####
#
# VleBuildAllDyn: for all the cpp files into the current directory,
#     if the tag "@@tagdynamic@@" is present then it builds a vle dynamic
#     using VleBuildDynamic.
#
#> set(VLE_ABI_VERSION 1.2)
#> find_package(VleUtils REQUIRED)
#> VleBuildAllDyn()
#
# ===========================================================================

set(VleUtils_VERSION 1.0)
if (NOT DEFINED VLE_DEBUG)
    message(FATAL_ERROR "VLE_DEBUG is not set ")
endif ()
if (NOT DEFINED VLE_ABI_VERSION)
    message(FATAL_ERROR "VLE_ABI_VERSION is not set ")
endif ()

##
## internal macros
##

macro(_vle_check_package_set var value)
  set(${var} "${value}" CACHE INTERNAL "")
endmacro(_vle_check_package_set)

macro(_vle_check_package_unset var)
  set(${var} "" CACHE INTERNAL "")
endmacro(_vle_check_package_unset)

###
# Split module requirement : 
#   in: ext.muparser(>=1.0)
#   out1: ext.muparser
#   out2: >=1.0
###

function(IntVleSplitModRequirement in out1 out2)
  if (VLE_DEBUG)
    message(STATUS "Enter IntVleSplitModRequirement : '${in}' ")
  endif () 
  string(REPLACE "(" ";" _module_as_list ${in})
  list(LENGTH _module_as_list _module_as_list_length)
  if (2 EQUAL _module_as_list_length)
    if (VLE_DEBUG)
      message(STATUS "Module '${in}' has a version requirement") 
    endif ()
    list(GET _module_as_list 0 _out1tmp)
    list(GET _module_as_list 1 _out2tmp)
    string(REPLACE ")" "" _out2tmp2 ${_out2tmp})
    set (${out1} ${_out1tmp} PARENT_SCOPE)
    set (${out2} ${_out2tmp2} PARENT_SCOPE) 
  else ()
    if (1 EQUAL _module_as_list_length)
      if (VLE_DEBUG)
        message(STATUS "Module '${in}' does not have a version requirement") 
      endif ()
      list(GET _module_as_list 0 _out1tmp)
      set (${out1} ${_out1tmp} PARENT_SCOPE)
      set (${out2} "" PARENT_SCOPE) 
    else ()
      message(FATAL_ERROR 
       "Module name format unrecognised : '${in}'")
   endif ()
  endif()
endfunction()

###
# Requirement check : 
#   in1: >=1.0
#   in2: 1.0.2
#   out: TRUE
###

function(IntVleRequirementCheck in1 in2 out)
   string(LENGTH ${in1} _in1length)
   string(SUBSTRING ${in1} 0 2 _in1twoChar)
   if (VLE_DEBUG)
        message(STATUS "IntVleRequirementCheck two-char comp. ${_in1twoChar}") 
        message(STATUS "IntVleRequirementCheck in1 ${in1}") 
   endif ()
   string (COMPARE EQUAL ${_in1twoChar} ">=" _greater_eq)
   if (${_greater_eq})
     math(EXPR _in1ver_length "${_in1length}-2")
     string(SUBSTRING ${in1} 2  ${_in1ver_length} _in1ver)
     if (VLE_DEBUG)
        message(STATUS "IntVleRequirementCheck vers. ${_in1ver}") 
     endif ()
     string(COMPARE EQUAL ${in2} ${_in1ver} _outtmp1)
     string(COMPARE GREATER ${in2} ${_in1ver} _outtmp2)
     if (_outtmp1 OR _outtmp2)
       set (${out} 1 PARENT_SCOPE)
     else ()
       set (${out} 0 PARENT_SCOPE)
     endif ()
     return ()
   endif ()
   string(SUBSTRING ${in1} 0 1 _in1oneChar)
   string (COMPARE EQUAL ${_in1oneChar} "=" _equal)
   if (${_equal})
     math(EXPR _in1ver_length "${_in1length}-1")
     string(SUBSTRING ${in1} 1  ${_in1ver_length} _in1ver)
     string(COMPARE EQUAL ${in2} ${_in1ver} _outtmp)
     if (_outtmp)
       set (${out} 1 PARENT_SCOPE)
     else ()
       set (${out} 0 PARENT_SCOPE)
     endif ()
     return ()
   endif ()
   message(FATAL_ERROR "Module version unrecognised : '${in1}'")
endfunction()

###
# Get the list of build depends of the current package : 
#   out: mypkg1,mypkg2(>=1.0)
###

function(IntVleBuildDepends out)
  if (NOT EXISTS "${CMAKE_SOURCE_DIR}/Description.txt")
    message(FATAL_ERROR "Missing Description.txt file: "
                        "${CMAKE_SOURCE_DIR}/Description.txt ")  
  endif ()
  file(STRINGS "${CMAKE_SOURCE_DIR}/Description.txt" _description_text)
  foreach(_line_descr_text IN LISTS _description_text)
   if ("${_line_descr_text}" MATCHES "^Build-Depends:(.*)")
    if (NOT ${CMAKE_MATCH_1} STREQUAL "")
      string(REGEX REPLACE "[ ]" "" _vle_pkgs_deps ${CMAKE_MATCH_1})
      string(REGEX REPLACE "," ";" _vle_pkgs_deps ${_vle_pkgs_deps})
      set (${out} "${_vle_pkgs_deps}" PARENT_SCOPE)
      return ()
    endif ()
   endif ()
  endforeach () 
endfunction()

###
# Get the VLE_HOME package directory: 
#   out: /home/user/.vle/pkgs-1.2
###

function(IntVleHomePkgs out)
  file(TO_CMAKE_PATH "$ENV{VLE_HOME}" _vle_home)
  if (NOT _vle_home)
    if (CMAKE_HOST_WIN32)
      file(TO_CMAKE_PATH "$ENV{HOMEDRIVE}$ENV{HOMEPATH}/vle" _vle_home)
    else ()
      file(TO_CMAKE_PATH "$ENV{HOME}/.vle" _vle_home)
    endif ()
    if (_vle_debug) 
      message(STATUS "The VLE_HOME undefined, try default ${_vle_home}")
    endif()
  else ()
    if (VLE_DEBUG)
      message(STATUS "The VLE_HOME defined: ${_vle_home}")
    endif ()
  endif ()
  if (NOT VLE_ABI_VERSION)
    set(_vle_package_dir "${_vle_home}/pkgs")
    if (VLE_DEBUG)
      message(STATUS "VLE_ABI_VERSION undefined for package directory, "
                     "try default ${_vle_package_dir}")
    endif()
  else ()
    set(_vle_package_dir "${_vle_home}/pkgs-${VLE_ABI_VERSION}")
    if (VLE_DEBUG)
      message(STATUS "VLE_ABI_VERSION defined for package directory: "
                     "${_vle_package_dir}")
    endif ()
  endif ()
  if (NOT IS_DIRECTORY ${_vle_package_dir})
    message(FATAL_ERROR "Package directory does not exist: ${_vle_package_dir}")
  endif ()
  set (${out} ${_vle_package_dir} PARENT_SCOPE)
endfunction()

###
# Get the the version of a installed package from the Description file:
#   in: mypkg 
#   out: 1.0.2
###

function(IntVleInstalledPkgVersion in out)
  IntVleHomePkgs(_vle_pkgs)
  if (NOT EXISTS "${_vle_pkgs}/${in}")
    message(FATAL_ERROR "Package: '${in}' is missing")
  endif ()
  set(_descr_file "${_vle_pkgs}/${in}/Description.txt")
  if (NOT EXISTS ${_descr_file})
    message(FATAL_ERROR "Description file does not exist: ${_descr_file}")
  endif ()
  file(STRINGS "${_descr_file}" _description_text)
  foreach(_line_descr_text IN LISTS _description_text)
   if ("${_line_descr_text}" MATCHES "^Version:(.*)")
    if (NOT ${CMAKE_MATCH_1} STREQUAL "")
      string(REGEX REPLACE "[ ]" "" _pkg_version ${CMAKE_MATCH_1})
      set (${out} "${_pkg_version}" PARENT_SCOPE)
    else ()
      set (${out} "" PARENT_SCOPE)
    endif ()
    return ()
   endif ()
  endforeach () 
endfunction()

######################
## user visible macros
######################

macro(VleCheckPackage _prefix _module_with_version)
  IntVleSplitModRequirement(${_module_with_version} _module _module_version)
  if (NOT ${_module_version} STREQUAL "")
    IntVleInstalledPkgVersion(${_module} _installed_mod_version)
    IntVleRequirementCheck(
      ${_module_version} ${_installed_mod_version} _version_ok)
  else ()
    set (_version_ok 1)
  endif ()
  if (NOT _version_ok)
    message(FATAL_ERROR "Version of '${module}' not compatible :
      required:'${_module_version}', got:'${_installed_mod_version}'")
  endif ()
  IntVleHomePkgs(_vle_package_dir)
  set(_vle_include_dirs "${_vle_package_dir}/${_module}/src")
  set(_vle_lib_dirs "${_vle_package_dir}/${_module}/lib")
  if (EXISTS "${_vle_include_dirs}" OR EXISTS "${_vle_lib_dirs}")
    if (WIN32)
      file(GLOB _vle_libraries "${_vle_lib_dirs}/*.a" "${_vle_lib_dirs}/*.dll")
    else ()
      file(GLOB _vle_libraries "${_vle_lib_dirs}/*.a")
    endif ()
    _vle_check_package_set(${_prefix}_FOUND 1)
    _vle_check_package_set(${_prefix}_INCLUDE_DIRS ${_vle_include_dirs})
    _vle_check_package_set(${_prefix}_LIBRARIES "${_vle_libraries}")
    if (_vle_debug)
      message(STATUS "Found `${_module}': ${_vle_libraries} and "
                       "${_vle_include_dirs}")
    endif ()
  else (EXISTS "${_vle_include_dirs}" OR EXISTS "${_vle_lib_dirs}")
    message(FATAL_ERROR "Package `${_module}' not found")
  endif (EXISTS "${_vle_include_dirs}" OR EXISTS "${_vle_lib_dirs}")
endmacro(VleCheckPackage)

## 

macro(VleCheckDependencies)
  IntVleBuildDepends(_vle_pkgs_deps)
  foreach(_vle_pkg_dep IN LISTS _vle_pkgs_deps)
    VleCheckPackage(${_vle_pkg_dep} ${_vle_pkg_dep})
    if (NOT ${${_vle_pkg_dep}_FOUND})
      message(FATAL_ERROR "Missing package ${__VLE_PKG_DEP}")
    endif ()
  endforeach ()
endmacro(VleCheckDependencies)

##

macro (VleBuildDynamic _dynname _cppfiles)
  if (DEFINED VLE_DEBUG)
    set (_vle_debug  ${VLE_DEBUG})
  else ()
    set (_vle_debug 0)
  endif ()
  set (__cppfilesarg "${_cppfiles}")
  list(GET __cppfilesarg 0 __cppfile1)
  file(READ "${__cppfile1}" __cppcontent)
  string(REGEX MATCH "@@tagdepends:(.+)@@endtagdepends" __tag_depends ${__cppcontent})
  set(__include_dirs "")
  set(__libraries "")
  if(NOT ${__tag_depends} STREQUAL "")
    set (__tag_depends ${CMAKE_MATCH_1})
    string(REGEX REPLACE "," ";" __tag_depends ${CMAKE_MATCH_1})
    foreach(__vle_pkg_dep IN LISTS __tag_depends)
      string(REGEX REPLACE "[ ]" "" __vle_pkg_dep ${__vle_pkg_dep})
      if (NOT ${__vle_pkg_dep} STREQUAL "")
        if (NOT ${__vle_pkg_dep}_FOUND)
          message (FATAL_ERROR "Missing pkg '${__vle_pkg_dep}' for building "
                               "'${__cppfile1}', maybe you forgot to add this " 
                               "package into the Description.txt file")
        endif ()
        set(__include_dirs "${__include_dirs};${${__vle_pkg_dep}_INCLUDE_DIRS}")
        set(__libraries "${__libraries};${${__vle_pkg_dep}_LIBRARIES}")
      endif ()    
    endforeach ()
    if (_vle_debug)
      message(STATUS "Additional include dir for `${_dynname}': "
                     "${__include_dirs}")
      message(STATUS "Additional libs to link for `${_dynname}': "
                     "${__libraries}")
    endif ()

  endif ()
  link_directories(${VLE_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
  add_library(${_dynname} MODULE ${__cppfilesarg})
  target_include_directories(${_dynname} PUBLIC
     "${CMAKE_SOURCE_DIR}/src;${VLE_INCLUDE_DIRS};"
     "${Boost_INCLUDE_DIRS};${__include_dirs}")
  target_link_libraries(${_dynname} "${__libraries};${VLE_LIBRARIES};"
                                    "${Boost_LIBRARIES}")
  install(TARGETS ${_dynname}
                 RUNTIME DESTINATION plugins/simulator
                 LIBRARY DESTINATION plugins/simulator)
endmacro(VleBuildDynamic)

##

macro (VleBuildTest _testname _cppfile)
  link_directories(
    ${VLE_LIBRARY_DIRS}
    ${Boost_LIBRARY_DIRS})
  add_executable(${_testname} ${_cppfile})
  target_include_directories(${_testname} PUBLIC
    "${CMAKE_SOURCE_DIR}/src;${VLE_INCLUDE_DIRS};${Boost_INCLUDE_DIRS}")
  target_link_libraries(${_testname}
    ${VLE_LIBRARIES}
    ${Boost_LIBRARIES}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY})
  add_test(${_testname} ${_testname})
endmacro (VleBuildTest)

##

macro (VleBuildOovPlugin _pluginname _cppfile)
  link_directories(${VLE_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
  add_library(${_pluginname} SHARED ${_cppfile})
  target_include_directories(${_pluginname} PUBLIC 
         "${CMAKE_SOURCE_DIR}/src;${VLE_INCLUDE_DIRS};${Boost_INCLUDE_DIRS};"
         "${CMAKE_BINARY_DIR}/src")
  target_link_libraries(${_pluginname} ${VLE_LIBRARIES})
  INSTALL(TARGETS ${_pluginname}
    RUNTIME DESTINATION plugins/output
    LIBRARY DESTINATION plugins/output)
endmacro (VleBuildOovPlugin)

##

macro (VleBuildAllDyn)
  if (DEFINED VLE_DEBUG)
    set (_vle_debug  ${VLE_DEBUG})
  else ()
    set (_vle_debug 0)
  endif ()
  file(GLOB __cpp_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
  foreach(__cppfile ${__cpp_files})
    file(READ "${__cppfile}" __cppcontent)
    string(REGEX MATCH "@@tagdynamic@@+" __tag_dyn ${__cppcontent})
    if(NOT ${__tag_dyn} STREQUAL "")
      string(REGEX MATCH "(.+).cpp" __libname ${__cppfile})
      set (__libname ${CMAKE_MATCH_1})
      if (_vle_debug)
        message(STATUS "Detected lib to build : `${__libname}'")
      endif ()
      VleBuildDynamic(${__libname} "${__cppfile}")
    endif()
  endforeach ()
endmacro (VleBuildAllDyn)

