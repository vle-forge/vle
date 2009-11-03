INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLADEMM_INCLUDE_DIRS}
  ${GTKMM_INCLUDE_DIRS}
  ${GTHREAD_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${GTKSOURCEVIEWMM_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLADEMM_LIBRARY_DIRS}
  ${GTKMM_LIBRARY_DIRS}
  ${GTHREAD_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${GTKSOURCEVIEWMM_LIBRARY_DIRS})

IF(WIN32)
  ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vfl.o
    COMMAND ${CMAKE_RC_COMPILER}
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/vfl.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
  SET(OS_SPECIFIC_PATH_IMPL ${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
ELSE(WIN32)
  SET(OS_SPECIFIC_PATH_IMPL)
ENDIF(WIN32)

SET(GVLE_HPP About.hpp AtomicModelBox.hpp BooleanBox.hpp CalendarBox.hpp
  ComboboxString.hpp ConditionsBox.hpp ConditionPlugin.hpp ConnectionBox.hpp
  CoupledModelBox.hpp CutCopyPaste.hpp DialogString.hpp DynamicBox.hpp
  Editor.hpp ExperimentBox.hpp GVLE.hpp GVLEMenuAndToolbar.hpp HelpBox.hpp
  HostsBox.hpp ImportClassesBox.hpp ImportModelBox.hpp InteractiveTypeBox.hpp
  LaunchSimulationBox.hpp MatrixBox.hpp Message.hpp ModelClassBox.hpp
  ModelDescriptionBox.hpp ModelingPlugin.hpp ModelTreeBox.hpp Modeling.hpp
  NewModelClassBox.hpp NewProjectBox.hpp NewTypeBox.hpp ObsAndViewBox.hpp
  ObservableBox.hpp ObserverPlugin.hpp OpenModelingPluginBox.hpp
  OpenPackageBox.hpp OpenVpzBox.hpp OutputPlugin.hpp ParameterExecutionBox.hpp
  PluginFactory.hpp PortDialog.hpp PreferencesBox.hpp QuitBox.hpp
  ReplicasBox.hpp SaveVpzBox.hpp SimpleTypeBox.hpp TableBox.hpp
  TreeViewConditions.hpp TreeViewObservable.hpp TreeViewValue.hpp UndoRedo.hpp
  ValueBox.hpp ValuesTreeView.hpp View.hpp ViewDrawingArea.hpp
  ViewOutputBox.hpp XmlTypeBox.hpp)

SET(GVLE_CPP About.cpp AtomicModelBox.cpp BooleanBox.cpp CalendarBox.cpp
  ConditionsBox.cpp ConditionPlugin.cpp ConnectionBox.cpp CoupledModelBox.cpp
  CutCopyPaste.cpp DialogString.cpp DynamicBox.cpp Editor.cpp ExperimentBox.cpp
  GVLE.cpp GVLEMenuAndToolbar.cpp HelpBox.cpp HostsBox.cpp ImportClassesBox.cpp
  ImportModelBox.cpp InteractiveTypeBox.cpp LaunchSimulationBox.cpp
  MatrixBox.cpp Message.cpp ModelClassBox.cpp ModelDescriptionBox.cpp
  ModelTreeBox.cpp Modeling.cpp ModelingPlugin.cpp NewModelClassBox.cpp
  NewProjectBox.cpp NewTypeBox.cpp ObsAndViewBox.cpp PluginFactory.cpp
  ObservableBox.cpp OpenModelingPluginBox.cpp OpenPackageBox.cpp OpenVpzBox.cpp
  OutputPlugin.cpp ParameterExecutionBox.cpp PortDialog.cpp PreferencesBox.cpp
  ReplicasBox.cpp QuitBox.cpp SaveVpzBox.cpp SimpleTypeBox.cpp TableBox.cpp
  TreeViewConditions.cpp TreeViewObservable.cpp TreeViewValue.cpp UndoRedo.cpp
  ValueBox.cpp ValuesTreeView.cpp View.cpp ViewDrawingArea.cpp
  ViewOutputBox.cpp XmlTypeBox.cpp ${OS_SPECIFIC_PATH_IMPL})

ADD_LIBRARY(vlegvle SHARED ${GVLE_CPP};${GVLE_HPP})

SET_TARGET_PROPERTIES(vlegvle PROPERTIES ${VLE_LIBRARY_PROPERTIES})
SET_TARGET_PROPERTIES(vlegvle PROPERTIES OUTPUT_NAME
  "vlegvle-${VLE_VERSION_SHORT}")

TARGET_LINK_LIBRARIES(vlegvle
  ${GLADEMM_LIBRARIES}
  ${GTKMM_LIBRARIES}
  ${GLIBMM_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${OS_SPECIFIC_LIBRARIES}
  ${GTKSOURCEVIEWMM_LIBRARIES}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vleoov
  vlevpz
  vledevs
  vleextension
  vlemanager
  vletranslator)

INSTALL(TARGETS vlegvle DESTINATION
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

INSTALL(FILES gvle.glade DESTINATION ${VLE_SHARE_DIRS}/glade)

INSTALL(FILES ${GVLE_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/gvle)

ADD_SUBDIRECTORY(outputs)
ADD_SUBDIRECTORY(conditions)
ADD_SUBDIRECTORY(modeling)
