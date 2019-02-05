cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(libgvle VERSION 2.1.0
  DESCRIPTION "Library for the GUI of the  Multi-Modeling and Simulation Environment VLE"
  LANGUAGES CXX)

set(libgvle_sources
  aboutbox.ui
  gvle_win.ui
  help.ui
  dom_tools.hpp
  vle_dom.hpp
  vlevpz.hpp
  aboutbox.h
  gvle_file.h
  gvle_plugins.h
  gvle_widgets.h
  gvle_win.h
  helpbrowser.h
  help.h
  logger.h
  notification.h
  plugin_cond.h
  plugin_mainpanel.h
  plugin_output.h
  plugin_simpanel.h
  aboutbox.cpp
  dom_tools.cpp
  gvle_file.cpp
  gvle_plugins.cpp
  gvle_widgets.cpp
  gvle_win.cpp
  helpbrowser.cpp
  help.cpp
  logger.cpp
  notification.cpp
  vle_dom.cpp
  vlevpz.cpp
  widgets/GvleCodeEdit.cpp
  widgets/GvleCodeEdit.h
  widgets/GvleTextEdit.cpp
  widgets/GvleTextEdit.h
  gvle.qrc)

if (WIN32)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gvle.o
    COMMAND ${CMAKE_RC_COMPILER}
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/gvle.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/gvle.o)

  list(APPEND libgvle_sources ${CMAKE_CURRENT_BINARY_DIR}/gvle.o)
endif (WIN32)

add_library(libgvle ${libgvle_sources})

option(BUILD_SHARED_LIBS "Build shared library" ON)
include(GenerateExportHeader)
generate_export_header(libgvle
    EXPORT_MACRO_NAME GVLE_API
    EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/vle/gvle/export.hpp)

target_compile_options(libgvle
  PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
      -pipe -march=native
      $<$<CONFIG:Debug>:-O0 -Wall -Wextra>>
  $<$<CXX_COMPILER_ID:MSVC>:
      $<$<CONFIG:Debug>:/Od /Wall /Zi>>)

target_compile_definitions(libgvle
  PRIVATE
  $<$<BOOL:${WITH_FULL_OPTIMIZATION}>:VLE_FULL_OPTIMIZATION>
  $<$<NOT:$<BOOL:${WITH_DEBUG}>>:VLE_DISABLE_DEBUG>
  $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
  $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>
  VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
  VERSION_MINOR=${PROJECT_VERSION_MINOR}
  VERSION_PATCH=${PROJECT_VERSION_PATCH})

target_include_directories(libgvle
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(libgvle PROPERTIES
  VERSION 0
  OUTPUT_NAME "gvle-${VLE_ABI}"
  POSITION_INDEPENDENT_CODE ON
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_link_libraries(libgvle
  PRIVATE
  Qt5::Widgets
  Qt5::Xml
  Qt5::Gui
  Qt5::Help
  Qt5::PrintSupport
  Qt5::Svg
  threads
  Boost::boost
  EXPAT::EXPAT
  libvle
  $<$<PLATFORM_ID:Linux>:dl>)

install(TARGETS libgvle
    EXPORT libgvle-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${LIBLEGACY_INCLUDE_DIRS})

install(EXPORT libgvle-targets
    FILE libgvle-targets.cmake
    NAMESPACE libgvle::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gvle-${VLE_ABI})

configure_file(gvle.pc.in
  ${CMAKE_BINARY_DIR}/src/gvle-${VLE_ABI}.pc
  @ONLY)

install(FILES
    ${CMAKE_BINARY_DIR}/src/gvle-${VLE_ABI}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

install(FILES
    dom_tools.hpp
    gvle_plugins.h
    gvle_win.h
    gvle_file.h
    gvle_widgets.h
    logger.h
    plugin_cond.h
    plugin_mainpanel.h
    plugin_output.h
    plugin_simpanel.h
    vle_dom.hpp
    vlevpz.hpp
    ${CMAKE_BINARY_DIR}/include/vle/gvle/export.hpp
    DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/vle-${VLE_ABI}/vle/gvle)

install(FILES
    widgets/GvleCodeEdit.h
    widgets/GvleTextEdit.h
    DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/vle-${VLE_ABI}/vle/gvle/widgets)
