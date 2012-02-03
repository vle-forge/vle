add_sources(vlelib About.cpp About.hpp AtomicModelBox.cpp AtomicModelBox.hpp
  BooleanBox.cpp BooleanBox.hpp CalendarBox.cpp CalendarBox.hpp
  ComboboxString.hpp CompleteViewDrawingArea.cpp CompleteViewDrawingArea.hpp
  ConditionsBox.cpp ConditionsBox.hpp ConnectionBox.cpp ConnectionBox.hpp
  CoupledModelBox.cpp CoupledModelBox.hpp CutCopyPaste.cpp CutCopyPaste.hpp
  DynamicBox.cpp DynamicBox.hpp DynamicsBox.cpp DynamicsBox.hpp Editor.cpp
  Editor.hpp ExperimentBox.cpp ExperimentBox.hpp FileTreeView.cpp
  FileTreeView.hpp GVLE.cpp GVLE.hpp GVLEMenuAndToolbar.cpp
  GVLEMenuAndToolbar.hpp HostsBox.cpp HostsBox.hpp ImportClassesBox.cpp
  ImportClassesBox.hpp ImportModelBox.cpp ImportModelBox.hpp
  InteractiveTypeBox.cpp InteractiveTypeBox.hpp LaunchSimulationBox.cpp
  LaunchSimulationBox.hpp MatrixBox.cpp MatrixBox.hpp Message.cpp Message.hpp
  ModelClassBox.cpp ModelClassBox.hpp ModelDescriptionBox.cpp
  ModelDescriptionBox.hpp Modeling.cpp Modeling.hpp ModelingPlugin.cpp
  ModelingPlugin.hpp ModelTreeBox.cpp ModelTreeBox.hpp NewDynamicsBox.cpp
  NewDynamicsBox.hpp NewModelClassBox.cpp NewModelClassBox.hpp NewProjectBox.cpp
  NewProjectBox.hpp NewTypeBox.cpp NewTypeBox.hpp ObsAndViewBox.cpp
  ObsAndViewBox.hpp ObserverPlugin.hpp OpenModelingPluginBox.cpp
  OpenModelingPluginBox.hpp OpenPackageBox.cpp OpenPackageBox.hpp OpenVpzBox.cpp
  OpenVpzBox.hpp OutputPlugin.cpp OutputPlugin.hpp PluginFactory.cpp
  PluginFactory.hpp PortDialog.cpp PortDialog.hpp PreferencesBox.cpp
  PreferencesBox.hpp QuitBox.cpp QuitBox.hpp SaveVpzBox.cpp SaveVpzBox.hpp
  Settings.cpp Settings.hpp SimpleTypeBox.cpp SimpleTypeBox.hpp
  SimpleViewDrawingArea.cpp SimpleViewDrawingArea.hpp TableBox.cpp TableBox.hpp
  TreeViewValue.cpp TreeViewValue.hpp TupleBox.cpp TupleBox.hpp ValueBox.cpp
  ValueBox.hpp ValuesTreeView.cpp ValuesTreeView.hpp View.cpp
  ViewDrawingArea.cpp ViewDrawingArea.hpp View.hpp ViewOutputBox.cpp
  ViewOutputBox.hpp XmlTypeBox.cpp XmlTypeBox.hpp)

install(FILES About.hpp AtomicModelBox.hpp BooleanBox.hpp CalendarBox.hpp
  ComboboxString.hpp CompleteViewDrawingArea.hpp ConditionsBox.hpp
  ConnectionBox.hpp CoupledModelBox.hpp CutCopyPaste.hpp DynamicBox.hpp
  DynamicsBox.hpp Editor.hpp ExperimentBox.hpp FileTreeView.hpp GVLE.hpp
  GVLEMenuAndToolbar.hpp HostsBox.hpp ImportClassesBox.hpp ImportModelBox.hpp
  InteractiveTypeBox.hpp LaunchSimulationBox.hpp MatrixBox.hpp Message.hpp
  ModelClassBox.hpp ModelDescriptionBox.hpp Modeling.hpp ModelingPlugin.hpp
  ModelTreeBox.hpp NewDynamicsBox.hpp NewModelClassBox.hpp NewProjectBox.hpp
  NewTypeBox.hpp ObsAndViewBox.hpp ObserverPlugin.hpp OpenModelingPluginBox.hpp
  OpenPackageBox.hpp OpenVpzBox.hpp OutputPlugin.hpp PluginFactory.hpp
  PortDialog.hpp PreferencesBox.hpp QuitBox.hpp SaveVpzBox.hpp Settings.hpp
  SimpleTypeBox.hpp SimpleViewDrawingArea.hpp TableBox.hpp TreeViewValue.hpp
  TupleBox.hpp ValueBox.hpp ValuesTreeView.hpp ViewDrawingArea.hpp View.hpp
  ViewOutputBox.hpp XmlTypeBox.hpp DESTINATION ${VLE_INCLUDE_DIRS}/gvle)

install(FILES gvle.glade DESTINATION ${VLE_SHARE_DIRS}/glade)
