CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(MyProject CXX C)
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
ENABLE_TESTING()

SET(MODEL_MAJOR 0)
SET(MODEL_MINOR 1)
SET(MODEL_PATCH 0)
SET(MODEL_NAME "${PROJECT_NAME}-${MODEL_MAJOR}.${MODEL_MINOR}.${MODEL_PATCH}")
SET(VLE_NAME "${PROJECT_NAME}-${MODEL_MAJOR}.${MODEL_MINOR}")

##
## Options for compilation of package
##

option(WITH_TEST "will build the test [default: ON]" ON)
option(WITH_DOC "will compile doc and install it [default: OFF]" OFF)
option(WITH_WARNINGS "will compile with g++ warnings [default: ON]" ON)
option(WITH_GVLE "use the GVLE [default: on]" ON)

##
## Flags
##

if (CMAKE_BUILD_TYPE STREQUAL "")
  SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
endif ()

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
  if (NOT WITH_WARNINGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
  else (NOT WITH_WARNINGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
  endif (NOT WITH_WARNINGS)
  if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR
      "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3")
  endif ()
endif ()

##
## Modules
##

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
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14/1y support.
                 Please use a different C++ compiler.")
endif()

##
## cmake policy
##
if (COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0020 NEW)#for qt link
endif (COMMAND CMAKE_POLICY)

##
## Check VLE and package dependencies
##

set(VLE_DEBUG 0)
set(VLE_ABI_VERSION 2.0)
find_package(VLE REQUIRED)
set (VleUtils_DIR "${VLE_SHARE_DIR}/cmake")
set (CMAKE_MODULE_PATH ${VLE_SHARE_DIR}/cmake ${CMAKE_MODULE_PATH})
find_package(VleUtils REQUIRED)
VleCheckDependencies()

##
## Check GVLE and package dependencies
##

if (WITH_GVLE)
  set(GVLE_DEBUG 0)
  find_package(GVLE)
  if (GVLE_FOUND)
    set (HAVE_GVLE 1 CACHE INTERNAL "" FORCE)
  else ()
    message (WARNING "GVLE not found!")
  endif ()
endif ()

##
## Find Qt
##

if (HAVE_GVLE)
  set(QMAKE_QUERY_LIBPATH qmake -query QT_INSTALL_LIBS)
  execute_process(COMMAND ${QMAKE_QUERY_LIBPATH} OUTPUT_VARIABLE
    CMAKE_PREFIX_PATH)
  find_package(Qt5 COMPONENTS Widgets Gui Xml)
  if (NOT Qt5_FOUND)
    message(FATAL_ERROR "Qt5 is required")
  endif (NOT Qt5_FOUND)
  if (NOT Qt5Widgets_FOUND)
    message(FATAL_ERROR "Qt5Widgets is required")
  endif (NOT Qt5Widgets_FOUND)
  if (NOT Qt5Gui_FOUND)
    message(FATAL_ERROR "Qt5Gui is required")
  endif (NOT Qt5Gui_FOUND)
  if (NOT Qt5Xml_FOUND)
    message(FATAL_ERROR "Qt5Xml is required")
  endif (NOT Qt5Xml_FOUND)
endif ()

##
## Generate the doxygen
##

if (WITH_DOC)
 FIND_PACKAGE(Doxygen)
 if (DOXYGEN)
  SET(DOXYGEN_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
  SET(DOXYGEN_OUTPUT_MODELING_DIR "${PROJECT_BINARY_DIR}/doxygen/modeling")
  SET(DOXYGEN_OUTPUT_SOURCE_DIR "${PROJECT_BINARY_DIR}/doxygen/sources")
  CONFIGURE_FILE("cmake/doxygen-modeling.conf.in"
    "${PROJECT_BINARY_DIR}/doxygen-modeling.conf")
  CONFIGURE_FILE("cmake/doxygen-sources.conf.in"
    "${PROJECT_BINARY_DIR}/doxygen-sources.conf")
  FILE(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/doxygen")
  FILE(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/doxygen/modeling")
  FILE(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/doxygen/sources")

  ADD_CUSTOM_COMMAND(
    OUTPUT "${PROJECT_BINARY_DIR}/doxygen/modeling/index.html"
    DEPENDS "${PROJECT_BINARY_DIR}/doxygen-modeling.conf"
    COMMAND "${DOXYGEN}"
    ARGS "${PROJECT_BINARY_DIR}/doxygen-modeling.conf")

  ADD_CUSTOM_COMMAND(
    OUTPUT "${PROJECT_BINARY_DIR}/doxygen/sources/index.html"
    DEPENDS "${PROJECT_BINARY_DIR}/doxygen-sources.conf"
    COMMAND "${DOXYGEN}"
    ARGS "${PROJECT_BINARY_DIR}/doxygen-sources.conf")

  ADD_CUSTOM_TARGET(doc_modeling ALL DEPENDS
    "${PROJECT_BINARY_DIR}/doxygen-modeling.conf"
    "${PROJECT_BINARY_DIR}/doxygen/modeling/index.html" VERBATIM)
  ADD_CUSTOM_TARGET(doc_sources ALL DEPENDS
    "${PROJECT_BINARY_DIR}/doxygen-sources.conf"
    "${PROJECT_BINARY_DIR}/doxygen/sources/index.html" VERBATIM)

  INSTALL(DIRECTORY "${PROJECT_BINARY_DIR}/doxygen/modeling/html" DESTINATION
    "doc/html/modeling")
  INSTALL(DIRECTORY "${PROJECT_BINARY_DIR}/doxygen/sources/html" DESTINATION
    "doc/html/sources")
 endif (DOXYGEN)
endif ()

##
## Subdirectory
##

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data")
  add_subdirectory(data)
endif ()
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/metadata")
  add_subdirectory(metadata)
endif ()
if (WITH_DOC)
  if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/doc")
    add_subdirectory(doc)
  endif ()
endif ()
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/exp")
  add_subdirectory(exp)
endif ()
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src")
  add_subdirectory(src)
endif ()
if (WITH_TEST)
  if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test")
    add_subdirectory(test)
  endif ()
endif ()

##
## CPack configuration
##

INSTALL(FILES Authors.txt Description.txt License.txt News.txt Readme.txt
  DESTINATION .)

INCLUDE(CMakeCPack.cmake)
