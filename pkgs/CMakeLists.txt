# DeclareSimulator defines system plug-ins for VLE.
function(Declare plugin targetname package name)
  add_library(${targetname} MODULE ${ARGN})

  target_compile_definitions(${targetname}
    PRIVATE
    $<$<BOOL:${WITH_FULL_OPTIMIZATION}>:VLE_FULL_OPTIMIZATION>
    $<$<NOT:$<BOOL:${WITH_DEBUG}>>:VLE_DISABLE_DEBUG>
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
    $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>
    VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    VERSION_MINOR=${PROJECT_VERSION_MINOR}
    VERSION_PATCH=${PROJECT_VERSION_PATCH})

  target_include_directories(${targetname}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})

  target_link_libraries(${targetname}
    PRIVATE
    libvle)

  set_target_properties(${targetname} PROPERTIES
    OUTPUT_NAME ${name}
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON)

  install(TARGETS ${targetname}
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/vle-${VLE_ABI}/pkgs/${package}/plugins/${plugin}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/vle-${VLE_ABI}/pkgs/${package}/plugins/${plugin}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/vle-${VLE_ABI}/pkgs/${package}/plugins/${plugin})
endfunction()

function(DeclareGvle plugin targetname package name)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTOUIC ON)

  add_library(${targetname} MODULE ${ARGN})

  target_compile_definitions(${targetname}
    PRIVATE
    $<$<BOOL:${WITH_FULL_OPTIMIZATION}>:VLE_FULL_OPTIMIZATION>
    $<$<NOT:$<BOOL:${WITH_DEBUG}>>:VLE_DISABLE_DEBUG>
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
    $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>
    VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    VERSION_MINOR=${PROJECT_VERSION_MINOR}
    VERSION_PATCH=${PROJECT_VERSION_PATCH}
    QT_PLUGIN)

  target_include_directories(${targetname}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/gvle>)

  target_link_libraries(${targetname}
    PRIVATE
    Qt5::Widgets
    Qt5::Xml
    Qt5::Gui
    Qt5::Help
    Qt5::PrintSupport
    Qt5::Svg)

  set_target_properties(${targetname} PROPERTIES
    OUTPUT_NAME ${name}
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON)

  install(TARGETS ${targetname}
    RUNTIME DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/vle-${VLE_ABI}/pkgs/${package}/plugins/gvle/${plugin}
    LIBRARY DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/vle-${VLE_ABI}/pkgs/${package}/plugins/gvle/${plugin}
    ARCHIVE DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/vle-${VLE_ABI}/pkgs/${package}/plugins/gvle/${plugin})
endfunction()

add_subdirectory(vle.output)
add_subdirectory(vle.adaptative-qss)
add_subdirectory(vle.generic.builder)

if (WITH_GVLE)
  add_subdirectory(gvle.default)
endif ()

# add_subdirectory(test)
