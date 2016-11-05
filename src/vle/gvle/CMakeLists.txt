set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDES}
  ${Qt5Gui_INCLUDES} ${Qt5Help_INCLUDES} ${Qt5PrintSupport_INCLUDES})

if (WIN32)
  set(INTL_LIB "intl")
else (WIN32)
  set(INTL_LIB "")
endif (WIN32)

include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS} ${gvle_SOURCE_DIR}
  ${VLE_SOURCE_DIR}/src/apps/gvle)


QT5_WRAP_UI(ui_gvle_win_h gvle_win.ui)
QT5_WRAP_UI(aboutbox_h aboutbox.ui)
QT5_WRAP_UI(help_h help.ui)

SET(gvle_SOURCES_HPP ${ui_gvle_win_h} ${aboutbox_h} ${help_h} gvle_widgets.h
   logger.h plugin_cond.h plugin_mainpanel.h plugin_simpanel.h plugin_output.h
   dom_tools.hpp vle_dom.hpp vlevpz.hpp gvle_plugins.h gvle_file.h)

SET(gvle_SOURCES_CPP gvle_widgets.cpp gvle_win.cpp aboutbox.cpp help.cpp
  helpbrowser.cpp logger.cpp dom_tools.cpp vle_dom.cpp vlevpz.cpp
  gvle_plugins.cpp gvle_file.cpp)

if (WIN32)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vfl.o
    COMMAND ${CMAKE_RC_COMPILER}
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/vfl.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
  set(OS_SPECIFIC_PATH_IMPL ${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
  set(OS_SPECIFIC_LIBRARIES ws2_32)
else (WIN32)
  set(OS_SPECIFIC_PATH_IMPL)
  set(OS_SPECIFIC_LIBRARIES dl)
endif (WIN32)

QT5_ADD_RESOURCES(GVLE_QRC gvle.qrc)

add_library(gvlelib SHARED ${gvle_SOURCES_CPP} ${gvle_SOURCES_HPP}
  ${GVLE_QRC})

set_target_properties(gvlelib PROPERTIES
  VERSION 0
  OUTPUT_NAME "gvle-${VLE_VERSION_SHORT}"
  DEFINE_SYMBOL "gvlelib_EXPORTS"
  COMPILE_DEFINITIONS "gvle_DLL"
#  COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden"
  CLEAN_DIRECT_OUTPUT 1)

if (WIN32)
  set_target_properties(gvlelib PROPERTIES
    LINK_FLAGS " -mwindows")
endif ()

add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Xml_DEFINITIONS}
  ${Qt5Gui_DEFINITIONS} ${Qt5Help_DEFINITIONS} ${Qt5PrintSupport_DEFINITIONS})

target_link_libraries(gvlelib vlelib ${INTL_LIB}
  ${Qt5Xml_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES}
  ${Qt5Help_LIBRARIES} ${Qt5PrintSupport_LIBRARIES})

install(FILES ${gvle_SOURCES_HPP}
  DESTINATION ${VLE_INCLUDE_DIRS}/gvle)

install(TARGETS gvlelib RUNTIME DESTINATION bin LIBRARY DESTINATION
  lib ARCHIVE DESTINATION lib)

#
# Configure the file gvle.pc.in to produce in the binary directory a
# filled pkg-config script gvle.pc.
#

if (WIN32)
  set(VLE_PKGCONFIG_PREFIXPATH "/target")
  set(VLE_PKGCONFIG_BOOSTINCLUDE_DIRS "\${includedir}")
  set(VLE_PKGCONFIG_LIBDIR "bin")
  set(VLE_GVLE_LIBRARY "-lgvle-${VLE_VERSION_SHORT}")
else()
  set(VLE_PKGCONFIG_PREFIXPATH "${CMAKE_INSTALL_PREFIX}")
  set(VLE_PKGCONFIG_BOOSTINCLUDE_DIRS "${Boost_INCLUDE_DIRS}")
  set(VLE_PKGCONFIG_LIBDIR "lib")
  set(VLE_GVLE_LIBRARY "")
endif()

configure_file(gvle.pc.in
  ${VLE_BINARY_DIR}/src/vle/gvle-${VLE_VERSION_SHORT}.pc @ONLY)

install(FILES ${VLE_BINARY_DIR}/src/vle/gvle-${VLE_VERSION_SHORT}.pc
  DESTINATION lib/pkgconfig)

if (VLE_HAVE_UNITTESTFRAMEWORK)
  add_subdirectory(test)
endif ()
