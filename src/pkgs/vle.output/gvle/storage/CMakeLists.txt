set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDES}
  ${Qt5Gui_INCLUDES} ${Qt5Help_INCLUDES} ${Qt5PrintSupport_INCLUDES})

include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS} ${gvle_SOURCE_DIR}
  ${VLE_SOURCE_DIR}/src/apps/gvle)

set(StoragePlugin_SOURCES StoragePluginGUI.cpp StoragePluginGUItab.cpp)

QT5_WRAP_UI(StoragePluginGvle_h StoragePluginGvle.ui)

ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS} ${Qt5Gui_DEFINITIONS}
  ${Qt5Xml_DEFINITIONS})

ADD_DEFINITIONS(-DQT_PLUGIN)

ADD_LIBRARY(gvleOutputStorage SHARED
  ${StoragePlugin_SOURCES}
  ${StoragePluginGvle_h} )

TARGET_LINK_LIBRARIES(gvleOutputStorage
  gvlelib ${Qt5Xml_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES}
  ${Qt5Help_LIBRARIES} ${Qt5PrintSupport_LIBRARIES} vlelib ${VLEDEPS_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT} ${OS_SPECIFIC_LIBRARIES})

INSTALL(TARGETS gvleOutputStorage 
  RUNTIME DESTINATION lib/vle-${VLE_VERSION_SHORT}/pkgs/vle.output/plugins/gvle/output
  LIBRARY DESTINATION lib/vle-${VLE_VERSION_SHORT}/pkgs/vle.output/plugins/gvle/output
  ARCHIVE DESTINATION lib/vle-${VLE_VERSION_SHORT}/pkgs/vle.output/plugins/gvle/output)
