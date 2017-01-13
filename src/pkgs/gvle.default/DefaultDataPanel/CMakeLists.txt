 set(CMAKE_INCLUDE_CURRENT_DIR ON)
 set(CMAKE_AUTOMOC ON)

##
## build DefaultCppPanel
##

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS} ${Qt5Help_INCLUDE_DIRS}
  ${Qt5PrintSupport_INCLUDE_DIRS})

include_directories(${VLE_BINARY_DIR}/src/pkgs/gvle.default/DefaultDataPanel
  ${VLE_SOURCE_DIR}/src ${VLEDEPS_INCLUDE_DIRS} ${VLE_SOURCE_DIR}/src/apps/gvle)

add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Gui_DEFINITIONS}
  ${Qt5Xml_DEFINITIONS})

add_definitions(-DQT_PLUGIN)

set(DefaultDataPanel_SOURCES_CPP DefaultDataPanel.cpp)

add_library(DefaultDataPanel SHARED ${DefaultDataPanel_SOURCES_CPP})

target_link_libraries(DefaultDataPanel
   gvlelib vlelib ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES}
   ${Qt5Xml_LIBRARIES})

set(DefaultDataPanel_INST
  lib/vle-${VLE_VERSION_SHORT}/pkgs/gvle.default/plugins/gvle/data)

install(TARGETS DefaultDataPanel
  RUNTIME DESTINATION ${DefaultDataPanel_INST}
  LIBRARY DESTINATION ${DefaultDataPanel_INST}
  ARCHIVE DESTINATION ${DefaultDataPanel_INST})
