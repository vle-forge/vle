set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(XMLDEPS libxml-2.0)
include_directories(${XMLDEPS_INCLUDE_DIRS})

include_directories(${Qt5Widgets_INCLUDES} ${Qt5Xml_INCLUDES}
  ${Qt5Gui_INCLUDES} ${Qt5Help_INCLUDES})

if (WIN32)
  set(INTL_LIB "intl")
else (WIN32)
  set(INTL_LIB "")
endif (WIN32)

include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS} ${gvle_SOURCE_DIR}
  ${VLE_SOURCE_DIR}/src/apps/gvle)

SET(gvle_SOURCES_CPP gvle_widgets.cpp gvle_win.cpp aboutbox.cpp help.cpp
  helpbrowser.cpp filevpzview.cpp filevpzdynamics.cpp  filevpzclasses.cpp
  filevpzobservables.cpp filevpzproject.cpp filevpzexpcond.cpp
  filevpzexpview.cpp simulation.cpp logger.cpp
  sourcecpp.cpp sourcecpptemplate.cpp vlepackage.cpp
  vleDomDiffStack.cpp
  vlevpz.cpp vlevpm.cpp vpzDiagScene.cpp widgetvpzproperty.cpp
  plugin_modeler.cpp pluginmodelerview.cpp pluginmodelersummary.cpp
  filevpzrtool.cpp gvle_plugins.cpp)

QT5_WRAP_UI(ui_gvle_win_h gvle_win.ui)
QT5_WRAP_UI(aboutbox_h aboutbox.ui)
QT5_WRAP_UI(help_h help.ui)
QT5_WRAP_UI(filevpzview_h     filevpzview.ui)
QT5_WRAP_UI(filevpzdynamics_h filevpzdynamics.ui)
QT5_WRAP_UI(filevpzproject_h filevpzproject.ui)
QT5_WRAP_UI(filevpzobservables_h filevpzobservables.ui)
QT5_WRAP_UI(filevpzexpcond_h  filevpzexpcond.ui)
QT5_WRAP_UI(filevpzexpview_h  filevpzexpview.ui)
QT5_WRAP_UI(filevpzclasses_h  filevpzclasses.ui)
QT5_WRAP_UI(filevpztooltip_h filevpztooltip.ui)
QT5_WRAP_UI(filevpzrtool_h   filevpzrtool.ui)
QT5_WRAP_UI(simulationview_h simulationview.ui)
QT5_WRAP_UI(simulationrtool_h simulationrtool.ui)
QT5_WRAP_UI(widgetvpzpropertydynamics_h widgetvpzpropertydynamics.ui)
QT5_WRAP_UI(widgetvpzpropertyexpcond_h  widgetvpzpropertyexpcond.ui)
QT5_WRAP_UI(widgetvpzpropertyobservables_h  widgetvpzpropertyobservables.ui)
QT5_WRAP_UI(pluginmodelerview_h pluginmodelerview.ui)
QT5_WRAP_UI(pluginmodelersummary_h pluginmodelersummary.ui)

SET(gvle_SOURCES_HPP ${ui_gvle_win_h} ${aboutbox_h} ${help_h}
  ${filevpzview_h} ${filevpzdynamics_h} ${filevpzobservables_h}
  ${filevpzproject_h} ${filevpzexpcond_h} ${filevpzexpview_h}
  ${filevpzclasses_h} ${filevpztooltip_h} ${filevpzrtool_h}
  ${simulationview_h} ${simulationrtool_h} gvle_widgets.h simulation.h
  logger.h sourcecpp.h sourcecpptemplate.h ${pluginmodelerview_h}
  pluginmodelerview.h ${pluginmodelersummary_h} pluginmodelersummary.h
  plugin_cond.h plugin_modeler.h plugin_sim.h plugin_output.h
  vlepackage.h vleDomDiffStack.h vlevpz.h vlevpm.h widgetvpzproperty.h
  vpzDiagScene.h gvle_plugins.h ${widgetvpzpropertydynamics_h}
  ${widgetvpzpropertyexpcond_h} ${widgetvpzpropertyobservables_h})

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
  ${Qt5Gui_DEFINITIONS} ${Qt5Help_DEFINITIONS})

target_link_libraries(gvlelib vlelib ${INTL_LIB}
  ${Qt5Xml_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES}
  ${Qt5Help_LIBRARIES})

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
