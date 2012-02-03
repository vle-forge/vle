add_sources(vlelib MainWindow.cpp MainWindow.hpp NetStreamReader.cpp
  NetStreamReader.hpp Plugin.cpp Plugin.hpp Window.cpp Window.hpp)

install(FILES MainWindow.hpp NetStreamReader.hpp Plugin.hpp Window.hpp
  DESTINATION ${VLE_INCLUDE_DIRS}/eov)

install(FILES eov.glade DESTINATION ${VLE_SHARE_DIRS}/glade)

add_subdirectory(plugins)
