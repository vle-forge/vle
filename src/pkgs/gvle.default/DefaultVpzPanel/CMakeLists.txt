 set(CMAKE_INCLUDE_CURRENT_DIR ON)
 set(CMAKE_AUTOMOC ON)

##
## build DefaultVpzPanel
## 

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS} ${Qt5Help_INCLUDE_DIRS}
  ${Qt5PrintSupport_INCLUDE_DIRS})

include_directories(${VLE_BINARY_DIR}/src/pkgs/gvle.default/DefaultVpzPanel
  ${VLE_SOURCE_DIR}/src ${VLEDEPS_INCLUDE_DIRS})

add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Gui_DEFINITIONS} 
                ${Qt5Xml_DEFINITIONS})
add_definitions(-DQT_PLUGIN)


QT5_WRAP_UI(filevpzview_h     filevpzview.ui)
QT5_WRAP_UI(filevpzdynamics_h filevpzdynamics.ui)
QT5_WRAP_UI(filevpzproject_h filevpzproject.ui)
QT5_WRAP_UI(filevpzobservables_h filevpzobservables.ui)
QT5_WRAP_UI(filevpzexpcond_h  filevpzexpcond.ui)
QT5_WRAP_UI(filevpzexpview_h  filevpzexpview.ui)
QT5_WRAP_UI(filevpzclasses_h  filevpzclasses.ui)
QT5_WRAP_UI(filevpzsim_h  filevpzsim.ui)
QT5_WRAP_UI(filevpztooltip_h filevpztooltip.ui)
QT5_WRAP_UI(filevpzrtool_h   filevpzrtool.ui)

set(DefaultVpzPanel_SOURCES_CPP filevpzdynamics.cpp filevpzclasses.cpp
  filevpzobservables.cpp filevpzproject.cpp filevpzexpcond.cpp filevpzview.cpp
  filevpzexpview.cpp vpzDiagScene.cpp widgetvpzproperty.cpp filevpzrtool.cpp
  filevpzsim.cpp DefaultVpzPanel.cpp)


add_library(DefaultVpzPanel SHARED ${DefaultVpzPanel_SOURCES_CPP}
  ${filevpzview_h} ${filevpzdynamics_h} ${filevpzproject_h} 
  ${filevpzobservables_h} ${filevpzexpcond_h} ${filevpzexpview_h}
  ${filevpzclasses_h} ${filevpzsim_h} ${filevpztooltip_h}
  ${filevpzrtool_h})
target_link_libraries(DefaultVpzPanel 
   gvlelib vlelib ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Xml_LIBRARIES})


set(DefaultVpzPanel_INST
  lib/vle-${VLE_VERSION_SHORT}/pkgs/gvle.default/plugins/gvle/vpz)
install(TARGETS DefaultVpzPanel 
  RUNTIME DESTINATION ${DefaultVpzPanel_INST}
  LIBRARY DESTINATION ${DefaultVpzPanel_INST}
  ARCHIVE DESTINATION ${DefaultVpzPanel_INST})
