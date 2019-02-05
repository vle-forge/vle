set(gvle_sources main.cpp)

if (WIN32)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vle.o
    COMMAND ${CMAKE_RC_COMPILER}
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/vle.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/vle.o)

  list(APPEND gvle_sources ${CMAKE_CURRENT_BINARY_DIR}/vle.o)
endif ()

add_executable(gvle ${gvle_sources})

target_include_directories(gvle
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/gvle>)

target_compile_definitions(gvle
  PRIVATE
  $<$<BOOL:${WITH_FULL_OPTIMIZATION}>:VLE_FULL_OPTIMIZATION>
  $<$<NOT:$<BOOL:${WITH_DEBUG}>>:VLE_DISABLE_DEBUG>
  $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
  $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>
  VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
  VERSION_MINOR=${PROJECT_VERSION_MINOR}
  VERSION_PATCH=${PROJECT_VERSION_PATCH})

set_target_properties(gvle
  PROPERTIES
  SOVERSION "${VLE_MAJOR}.${VLE_MINOR}"
  VERSION "${VLE_MAJOR}.${VLE_MINOR}"
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON)

target_link_libraries(gvle
  PRIVATE
  libvle
  libgvle
  threads
  Boost::boost
  EXPAT::EXPAT
  $<$<PLATFORM_ID:Linux>:dl>
  Qt5::Widgets
  Qt5::Xml
  Qt5::Gui
  Qt5::Help
  Qt5::PrintSupport
  Qt5::Svg)

install(TARGETS gvle DESTINATION bin)
