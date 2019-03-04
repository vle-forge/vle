find_package(QCustomPlot)

if (NOT QCustomPlot_FOUND)
  message("Plugin DefaultsimSubpanel disabled. Please insteall QCustomplot package.")
else ()
  add_library(DefaultSimSubpanel MODULE DefaultSimSubpanel.cpp
    DefaultSimSubpanel.h simpanelleft.ui simpanelright.ui)

  target_compile_definitions(DefaultSimSubpanel
    PRIVATE
    $<$<BOOL:${WITH_FULL_OPTIMIZATION}>:VLE_FULL_OPTIMIZATION>
    $<$<NOT:$<BOOL:${WITH_DEBUG}>>:VLE_DISABLE_DEBUG>
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
    $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>
    VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    VERSION_MINOR=${PROJECT_VERSION_MINOR}
    VERSION_PATCH=${PROJECT_VERSION_PATCH}
    QT_PLUGIN)

  target_include_directories(DefaultSimSubpanel
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/gvle>
    ${QCustomPlot_INCLUDE_DIR})

  target_link_libraries(DefaultSimSubpanel
    PUBLIC
    libvle
    libgvle
    Qt5::Widgets
    Qt5::Xml
    Qt5::Gui
    Qt5::Help
    Qt5::PrintSupport
    Qt5::Svg
    ${QCustomPlot_LIBRARIES})

  set_target_properties(DefaultSimSubpanel PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON)

  install(TARGETS DefaultSimSubpanel
    RUNTIME DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/vle-${VLE_ABI}/pkgs/gvle.default/plugins/gvle/simulating
    LIBRARY DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/vle-${VLE_ABI}/pkgs/gvle.default/plugins/gvle/simulating
    ARCHIVE DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/vle-${VLE_ABI}/pkgs/gvle.default/plugins/gvle/simulating)
endif ()
