set(GVLE_SOURCES_CPP About.cpp AtomicModelBox.cpp BooleanBox.cpp
  CalendarBox.cpp CompleteViewDrawingArea.cpp ConditionsBox.cpp
  ConnectionBox.cpp CoupledModelBox.cpp CutCopyPaste.cpp
  DynamicBox.cpp DynamicsBox.cpp Editor.cpp ExperimentBox.cpp
  FileTreeView.cpp GVLE.cpp GVLEMenuAndToolbar.cpp
  ImportClassesBox.cpp ImportModelBox.cpp InteractiveTypeBox.cpp
  LaunchSimulationBox.cpp MatrixBox.cpp Message.cpp ModelClassBox.cpp
  ModelDescriptionBox.cpp Modeling.cpp GlobalPlugin.cpp
  ModelingPlugin.cpp ModelTreeBox.cpp NewDynamicsBox.cpp
  NewModelClassBox.cpp NewTypeBox.cpp ObsAndViewBox.cpp
  OpenModelingPluginBox.cpp OpenPackageBox.cpp OutputPlugin.cpp
  PluginFactory.cpp PortDialog.cpp PreferencesBox.cpp QuitBox.cpp
  SaveVpzBox.cpp Settings.cpp SimpleTypeBox.cpp
  SimpleViewDrawingArea.cpp TableBox.cpp TreeViewValue.cpp
  TupleBox.cpp ValueBox.cpp ValuesTreeView.cpp View.cpp
  ViewDrawingArea.cpp ViewOutputBox.cpp XmlTypeBox.cpp SpawnPool.cpp)

set(GVLE_SOURCES_HPP About.hpp AtomicModelBox.hpp BooleanBox.hpp
  CalendarBox.hpp ComboboxString.hpp CompleteViewDrawingArea.hpp
  ConditionsBox.hpp ConnectionBox.hpp CoupledModelBox.hpp
  CutCopyPaste.hpp DllDefines.hpp DynamicBox.hpp DynamicsBox.hpp
  Editor.hpp ExperimentBox.hpp FileTreeView.hpp GVLE.hpp
  GVLEMenuAndToolbar.hpp ImportClassesBox.hpp ImportModelBox.hpp
  InteractiveTypeBox.hpp LaunchSimulationBox.hpp MatrixBox.hpp
  Message.hpp ModelClassBox.hpp ModelDescriptionBox.hpp Modeling.hpp
  GlobalPlugin.hpp ModelingPlugin.hpp ModelTreeBox.hpp
  NewDynamicsBox.hpp NewModelClassBox.hpp NewTypeBox.hpp
  ObsAndViewBox.hpp ObserverPlugin.hpp OpenModelingPluginBox.hpp
  OpenPackageBox.hpp OutputPlugin.hpp PluginFactory.hpp PortDialog.hpp
  PreferencesBox.hpp QuitBox.hpp SaveVpzBox.hpp Settings.hpp
  SimpleTypeBox.hpp SimpleViewDrawingArea.hpp TableBox.hpp
  TreeViewValue.hpp TupleBox.hpp ValueBox.hpp ValuesTreeView.hpp
  ViewDrawingArea.hpp View.hpp ViewOutputBox.hpp XmlTypeBox.hpp
  SpawnPool.hpp)

install(FILES ${GVLE_SOURCES_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/gvle)

install(FILES gvle.glade DESTINATION ${VLE_SHARE_DIRS}/glade)

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

add_library(gvlelib SHARED
    ${GVLE_SOURCES_HPP};${GVLE_SOURCES_CPP};${OS_SPECIFIC_PATH_IMPL})

set_target_properties(gvlelib PROPERTIES
  VERSION 0
  OUTPUT_NAME "gvle-${VLE_VERSION_SHORT}"
  DEFINE_SYMBOL "gvlelib_EXPORTS"
  COMPILE_DEFINITIONS "GVLE_DLL"
  COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden"
  CLEAN_DIRECT_OUTPUT 1)

if (WIN32)
  set_target_properties(gvlelib PROPERTIES
    LINK_FLAGS " -mwindows")
endif ()

target_link_libraries(gvlelib vlelib ${VLEDEPS_LIBRARIES}
  ${Boost_LIBRARIES} ${OS_SPECIFIC_LIBRARIES})

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
