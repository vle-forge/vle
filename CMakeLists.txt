cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(VLE VERSION 2.1.0
  DESCRIPTION "A Multi-Modeling and Simulation Environment"
  LANGUAGES CXX)

# Use the GNU standard installation directories. See
# https://cmake.org/cmake/help/v3.0/module/GNUInstallDirs.html
include(GNUInstallDirs)

# Options
option(WITH_FULL_OPTIMIZATION "disable all logging facilities and active heavy optimization code to speed up simulation. [default: off]" OFF)
option(WITH_DEBUG "enable debug log message. It slows simulation [default: ON]" ON)
option(WITH_GVLE "use QT to build gvle [default: on]" ON)
option(WITH_DOXYGEN "build the documentation with doxygen [default: off]" OFF)
option(WITH_CVLE "build cvle [default: on]" ON)

# Usefull variables
set(VLE_MAJOR ${PROJECT_VERSION_MAJOR})
set(VLE_MINOR ${PROJECT_VERSION_MINOR})
set(VLE_PATCH ${PROJECT_VERSION_PATCH})
set(VLE_VERSION "${VLE_MAJOR}.${VLE_MINOR}.${VLE_PATCH}")
set(VLE_ABI "${VLE_MAJOR}.${VLE_MINOR}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(EXPAT REQUIRED)
if (NOT EXPAT_FOUND)
  message(FATAL_ERROR "expat library is required. Install libexpat first (libexpat1-dev Debian/Ubuntu)")
endif ()

find_package(Boost REQUIRED)
if (NOT Boost_FOUND)
  message(FATAL_ERROR "boost header only library is required. Install boost-dev first (libboost-dev Debian/Ubuntu)")
endif ()

find_package(EXPAT REQUIRED)
if (NOT EXPAT_FOUND)
  message(FATAL_ERROR "expat library is required. Install libexpat first (libexpat1-dev Debian/Ubuntu)")
endif ()

find_package(Threads REQUIRED)
add_library(threads INTERFACE IMPORTED)
set_property(TARGET threads PROPERTY
  INTERFACE_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

if (WITH_GVLE)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTOUIC ON)
  set(CMAKE_AUTORCC ON)
  find_package(Qt5 COMPONENTS Widgets Xml Gui Help PrintSupport Svg REQUIRED)
  if (NOT Qt5_FOUND)
    message(FATAL_ERROR "Qt5 is required for gvle. Install Widgets Xml Gui Help PrintSupport Svg")
  endif ()
endif ()

if (WITH_CVLE)
  find_package(MPI REQUIRED)
  if (NOT MPI_FOUND)
    message(FATAL_ERROR "MPI is required for cvle. Install openmpi, mpich2")
  endif ()
endif ()

# Tries to found asciidoctor to generate manpages of the share/doc directory.
find_program(ASCIIDOCTOR_EXECUTABLE NAMES asciidoctor)
if (ASCIIDOCTOR_EXECUTABLE)
  message(STATUS "asciidoctor bin ${ASCIIDOCTOR_EXECUTABLE}")
else ()
  message(STATUS "asciidoctor not found.")
endif ()

if (UNIX)
  install(FILES README.md
    DESTINATION ${CMAKE_INSTALL_DATADIR}/vle-${VLE_ABI}/doc
    RENAME README)
  install(FILES COPYING
    DESTINATION ${CMAKE_INSTALL_DATADIR}/vle-${VLE_ABI}/doc)
else ()
  install(FILES README.md
    DESTINATION ${CMAKE_INSTALL_DATADIR}/vle-${VLE_ABI}/doc
    RENAME Readme.txt)
  install(FILES COPYING
    DESTINATION ${CMAKE_INSTALL_DATADIR}/vle-${VLE_ABI}/doc
    RENAME CopyRight.txt)
endif ()

# Gettext utilities
# find_package(Gettext)
# if (GETTEXT_FOUND)
#   set(VLE_HAVE_NLS 1 CACHE INTERNAL "" FORCE)
#   add_subdirectory(i18n)
# else (GETTEXT_FOUND)
#   set(VLE_HAVE_NLS 0 CACHE INTERNAL "" FORCE)
# endif (GETTEXT_FOUND)

# Generating documentation with doxygen
if (WITH_DOXYGEN)
  find_package(Doxygen)
  if (DOXYGEN)
    add_subdirectory(doxygen)
  else (DOXYGEN)
    message(STATUS "WARNING: Doxygen not found - No reference manual create")
  endif (DOXYGEN)
endif (WITH_DOXYGEN)


# Build a generic make uninstall command
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

message(STATUS "- - - -")
message(STATUS "VLE ${VLE_VERSION} configured successfully")
message(STATUS "Using ${CMAKE_INSTALL_PREFIX} for installation")
message(STATUS "Build type ${CMAKE_BUILD_TYPE}")
message(STATUS "Full optimization.............: ${WITH_FULL_OPTIMIZATION}")
message(STATUS "Show debug message............. ${WITH_DEBUG}")
message(STATUS "Build with gvle...............: ${WITH_GVLE}")
message(STATUS "Build with cvle...............: ${WITH_CVLE}")

enable_testing()
add_subdirectory(src)
add_subdirectory(pkgs)
add_subdirectory(apps)
add_subdirectory(tests)
add_subdirectory(share)
include(CMakeCPack.cmake)
