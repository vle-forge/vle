set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(QT_USE_QTXML TRUE)
set(QT_USE_QTHELP TRUE)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package(PkgConfig REQUIRED)

pkg_check_modules(XMLDEPS libxml-2.0)
include_directories(${XMLDEPS_INCLUDE_DIRS})

if (WIN32)
  set(INTL_LIB "intl")
else (WIN32)
  set(INTL_LIB "")
endif (WIN32)

include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS} ${GVLE2_SOURCE_DIR}
  ${VLE_SOURCE_DIR}/src/apps/gvle2)

SET(GVLE2_SOURCES_CPP gvle2win.cpp aboutbox.cpp help.cpp
  helpbrowser.cpp filevpzview.cpp filevpzdynamics.cpp
  filevpzobservables.cpp filevpzproject.cpp filevpzexpcond.cpp
  filevpzexpview.cpp simulation.cpp widgetprojecttree.cpp logger.cpp
  sourcecpp.cpp sourcecpptemplate.cpp vlepackage.cpp vpzActions.cpp
  vlevpz.cpp vlevpzconn.cpp vlevpzdynamic.cpp vlevpzport.cpp
  widgetvpzproperty.cpp plugin_modeler.cpp pluginmodelerview.cpp
  pluginmodelersummary.cpp )

QT4_WRAP_UI(ui_gvle2win_h gvle2win.ui)
QT4_WRAP_UI(aboutbox_h aboutbox.ui)
QT4_WRAP_UI(help_h help.ui)
QT4_WRAP_UI(filevpzview_h     filevpzview.ui)
QT4_WRAP_UI(filevpzdynamics_h filevpzdynamics.ui)
QT4_WRAP_UI(filevpzproject_h filevpzproject.ui)
QT4_WRAP_UI(filevpzobservables_h filevpzobservables.ui)
QT4_WRAP_UI(filevpzexpcond_h  filevpzexpcond.ui)
QT4_WRAP_UI(filevpzexpview_h  filevpzexpview.ui)
QT4_WRAP_UI(filevpztooltip_h filevpztooltip.ui)
QT4_WRAP_UI(filevpzrtool_h   filevpzrtool.ui)
QT4_WRAP_UI(simulationview_h simulationview.ui)
QT4_WRAP_UI(simulationrtool_h simulationrtool.ui)
QT4_WRAP_UI(widgetvpzpropertydynamics_h widgetvpzpropertydynamics.ui)
QT4_WRAP_UI(widgetvpzpropertyexpcond_h  widgetvpzpropertyexpcond.ui)
QT4_WRAP_UI(widgetvpzpropertyobservables_h  widgetvpzpropertyobservables.ui)
QT4_WRAP_UI(pluginmodelerview_h pluginmodelerview.ui)
QT4_WRAP_UI(pluginmodelersummary_h pluginmodelersummary.ui)

SET(GVLE2_SOURCES_HPP ${ui_gvle2win_h} ${aboutbox_h} ${help_h}
  ${filevpzview_h} ${filevpzdynamics_h} ${filevpzobservables_h}
  ${filevpzproject_h} ${filevpzexpcond_h} ${filevpzexpview_h}
  ${filevpztooltip_h} ${filevpzrtool_h} ${simulationview_h}
  ${simulationrtool_h} simulation.h logger.h sourcecpp.h
  sourcecpptemplate.h ${pluginmodelerview_h} pluginmodelerview.h
  ${pluginmodelersummary_h} pluginmodelersummary.h plugin_cond.h
  plugin_modeler.h plugin_sim.h plugin_output.h vlepackage.h vlevpz.h
  vlevpzconn.h vlevpzdynamic.h vlevpzport.h widgetvpzproperty.h
  ${widgetvpzpropertydynamics_h} ${widgetvpzpropertyexpcond_h}
  ${widgetvpzpropertyobservables_h})

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

add_library(gvle2lib SHARED ${GVLE2_SOURCES_CPP} ${GVLE2_SOURCES_HPP})

set_target_properties(gvle2lib PROPERTIES
  VERSION 0
  OUTPUT_NAME "gvle2-${VLE_VERSION_SHORT}"
  DEFINE_SYMBOL "gvle2lib_EXPORTS"
  COMPILE_DEFINITIONS "GVLE2_DLL"
#  COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden"
  CLEAN_DIRECT_OUTPUT 1)

if (WIN32)
  set_target_properties(gvle2lib PROPERTIES
    LINK_FLAGS " -mwindows")
endif ()

target_link_libraries(gvle2lib vlelib ${QT_LIBRARIES} ${INTL_LIB})

install(FILES ${GVLE2_SOURCES_HPP}
  DESTINATION ${VLE_INCLUDE_DIRS}/gvle2)

install(TARGETS gvle2lib RUNTIME DESTINATION bin LIBRARY DESTINATION
  lib ARCHIVE DESTINATION lib)

#
# Configure the file gvle.pc.in to produce in the binary directory a
# filled pkg-config script gvle.pc.
#

if (WIN32)
  set(VLE_PKGCONFIG_PREFIXPATH "/target")
  set(VLE_PKGCONFIG_BOOSTINCLUDE_DIRS "\${includedir}")
  set(VLE_PKGCONFIG_LIBDIR "bin")
  set(VLE_GVLE_LIBRARY "-lgvle2-${VLE_VERSION_SHORT}")
else()
  set(VLE_PKGCONFIG_PREFIXPATH "${CMAKE_INSTALL_PREFIX}")
  set(VLE_PKGCONFIG_BOOSTINCLUDE_DIRS "${Boost_INCLUDE_DIRS}")
  set(VLE_PKGCONFIG_LIBDIR "lib")
  set(VLE_GVLE_LIBRARY "")
endif()

configure_file(gvle2.pc.in
  ${VLE_BINARY_DIR}/src/vle/gvle2-${VLE_VERSION_SHORT}.pc @ONLY)

install(FILES ${VLE_BINARY_DIR}/src/vle/gvle2-${VLE_VERSION_SHORT}.pc
  DESTINATION lib/pkgconfig)
