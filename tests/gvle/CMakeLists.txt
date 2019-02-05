vle_declare_test(test_dom test_dom.cpp)

target_include_directories(test_dom
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/gvle>)

target_link_libraries(test_dom
  PRIVATE
  Qt5::Widgets
  Qt5::Xml
  Qt5::Gui
  Qt5::Help
  Qt5::PrintSupport
  Qt5::Svg
  threads
  Boost::boost
  EXPAT::EXPAT
  libvle
  libgvle
  $<$<PLATFORM_ID:Linux>:dl>)
