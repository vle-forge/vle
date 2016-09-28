 set(CMAKE_INCLUDE_CURRENT_DIR ON)
 set(CMAKE_AUTOMOC ON)
 
##
## build DefaultSimSubpanel
##

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS} ${Qt5Help_INCLUDE_DIRS}
  ${Qt5PrintSupport_INCLUDE_DIRS})

include_directories(${VLE_BINARY_DIR}/src/pkgs/gvle.default 
  ${VLE_SOURCE_DIR}/src ${VLEDEPS_INCLUDE_DIRS} ${VLE_SOURCE_DIR}/src/apps/gvle)

add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Gui_DEFINITIONS} 
                ${Qt5Xml_DEFINITIONS})
add_definitions(-DQT_PLUGIN)

QT5_WRAP_UI(simpanelleft_h simpanelleft.ui)
QT5_WRAP_UI(simpanelright_h simpanelright.ui)

set(DefaultSimSubpanel_SOURCES_CPP DefaultSimSubpanel.cpp
    ${simpanelleft_h} ${simpanelright_h})

add_library(DefaultSimSubpanel SHARED ${DefaultSimSubpanel_SOURCES_CPP}
            ${simpanelleft_h} ${simpanelright_h} qcustomplot/qcustomplot.cpp)

target_link_libraries(DefaultSimSubpanel vlelib gvlelib
  ${Qt5Xml_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES}
  ${Qt5Help_LIBRARIES} ${Qt5PrintSupport_LIBRARIES})

set(DefaultSimSubpanel_INST
  lib/vle-${VLE_VERSION_SHORT}/pkgs/gvle.default/plugins/gvle/simulating)
install(TARGETS DefaultSimSubpanel 
  RUNTIME DESTINATION ${DefaultSimSubpanel_INST}
  LIBRARY DESTINATION ${DefaultSimSubpanel_INST}
  ARCHIVE DESTINATION ${DefaultSimSubpanel_INST})
