 set(CMAKE_INCLUDE_CURRENT_DIR ON)
 set(CMAKE_AUTOMOC ON)

##
## build DateCondition
##

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS} ${Qt5Help_INCLUDE_DIRS}
  ${Qt5PrintSupport_INCLUDE_DIRS})

include_directories(${VLE_BINARY_DIR}/src/pkgs/gvle.default/DateCondition 
  ${VLE_SOURCE_DIR}/src ${VLEDEPS_INCLUDE_DIRS} ${VLE_SOURCE_DIR}/src/apps/gvle)

add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Gui_DEFINITIONS} 
                ${Qt5Xml_DEFINITIONS})
                
add_definitions(-DQT_PLUGIN)

set(DateCond_SOURCES DateCondition.cpp)

add_library(DateCondition SHARED ${DateCond_SOURCES})

target_link_libraries(DateCondition 
   gvlelib vlelib ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES} 
   ${Qt5Xml_LIBRARIES})

set(DateCondition_INST
  lib/vle-${VLE_VERSION_SHORT}/pkgs/gvle.default/plugins/gvle/condition)
install(TARGETS DateCondition 
  RUNTIME DESTINATION ${DateCondition_INST}
  LIBRARY DESTINATION ${DateCondition_INST}
  ARCHIVE DESTINATION ${DateCondition_INST})