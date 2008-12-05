INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLADEMM_INCLUDE_DIRS}
  ${GTKMM_INCLUDE_DIRS}
  ${GTHREAD_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLADEMM_LIBRARY_DIRS}
  ${GTKMM_LIBRARY_DIRS}
  ${GTHREAD_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR})

SET(GVLE_HPP About.hpp AtomicModelBox.hpp BooleanBox.hpp CalendarBox.hpp
  ClassModelTreeBox.hpp ComboboxString.hpp ConditionsBox.hpp ConnectionBox.hpp
  CoupledModelBox.hpp CutCopyPaste.hpp Data.hpp DialogString.hpp DynamicBox.hpp
  ExecutionBox.hpp ExperimentBox.hpp GVLE.hpp GVLEMenu.hpp HelpBox.hpp
  HostsBox.hpp ImportModelBox.hpp LaunchSimulationBox.hpp MatrixBox.hpp
  Message.hpp ModelClassBox.hpp ModelDescriptionBox.hpp ModelTreeBox.hpp
  Modeling.hpp NewModelClassBox.hpp NewTypeBox.hpp ObsAndViewBox.hpp
  ObservableBox.hpp ObserverPlugin.hpp Paned.hpp ParameterExecutionBox.hpp
  Plugin.hpp PluginPlus.hpp PluginSelection.hpp PluginTable.hpp PortDialog.hpp
  ReplicasBox.hpp SimpleTypeBox.hpp TableBox.hpp
  TreeViewConditions.hpp TreeViewObservable.hpp TreeViewValue.hpp UndoRedo.hpp
  ValueBox.hpp ValuesTreeView.hpp View.hpp ViewDrawingArea.hpp ViewMenu.hpp
  ViewOutputBox.hpp XmlTypeBox.hpp)

SET(GVLE_CPP About.cpp AtomicModelBox.cpp BooleanBox.cpp CalendarBox.cpp
  ClassModelTreeBox.cpp ConditionsBox.cpp ConnectionBox.cpp CoupledModelBox.cpp
  CutCopyPaste.cpp Data.cpp DialogString.cpp DynamicBox.cpp ExecutionBox.cpp
  ExperimentBox.cpp GVLE.cpp GVLEMenu.cpp HelpBox.cpp HostsBox.cpp
  ImportModelBox.cpp LaunchSimulationBox.cpp MatrixBox.cpp Message.cpp
  ModelClassBox.cpp ModelDescriptionBox.cpp ModelTreeBox.cpp Modeling.cpp
  NewModelClassBox.cpp NewTypeBox.cpp ObsAndViewBox.cpp ObservableBox.cpp
  ParameterExecutionBox.cpp PluginSelection.cpp PluginTable.cpp PortDialog.cpp
  ReplicasBox.cpp SimpleTypeBox.cpp TableBox.cpp
  TreeViewConditions.cpp TreeViewObservable.cpp TreeViewValue.cpp UndoRedo.cpp
  ValueBox.cpp ValuesTreeView.cpp View.cpp ViewDrawingArea.cpp ViewMenu.cpp
  ViewOutputBox.cpp XmlTypeBox.cpp)

ADD_LIBRARY(vlegvle SHARED ${GVLE_CPP};${GVLE_HPP})

SET_TARGET_PROPERTIES(vlegvle PROPERTIES ${VLE_APP_PROPERTIES})

TARGET_LINK_LIBRARIES(vlegvle
  ${GLADEMM_LIBRARIES}
  ${GTKMM_LIBRARIES}
  ${GLIBMM_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${OS_SPECIFIC_LIBRARIES}
  vlegeometry
  vleutils
  vledata
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
