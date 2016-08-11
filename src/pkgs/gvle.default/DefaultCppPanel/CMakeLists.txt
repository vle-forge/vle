 set(CMAKE_INCLUDE_CURRENT_DIR ON)
 set(CMAKE_AUTOMOC ON)

##
## build DefaultCppPanel
##

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS} ${Qt5Help_INCLUDE_DIRS}
  ${Qt5PrintSupport_INCLUDE_DIRS})

include_directories(${VLE_BINARY_DIR}/src/pkgs/gvle.default/DefaultCppPanel 
  ${VLE_SOURCE_DIR}/src ${VLEDEPS_INCLUDE_DIRS} ${VLE_SOURCE_DIR}/src/apps/gvle)

add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Gui_DEFINITIONS} 
                ${Qt5Xml_DEFINITIONS})
add_definitions(-DQT_PLUGIN)

set(DefaultCppPanel_SOURCES_CPP DefaultCppPanel.cpp)

add_library(DefaultCppPanel SHARED ${DefaultCppPanel_SOURCES_CPP})

target_link_libraries(DefaultCppPanel 
   gvlelib vlelib ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES} 
   ${Qt5Xml_LIBRARIES})

set(DefaultCppPanel_INST
  lib/vle-${VLE_VERSION_SHORT}/pkgs/gvle.default/plugins/gvle/modeling)
install(TARGETS DefaultCppPanel 
  RUNTIME DESTINATION ${DefaultCppPanel_INST}
  LIBRARY DESTINATION ${DefaultCppPanel_INST}
  ARCHIVE DESTINATION ${DefaultCppPanel_INST})
            

