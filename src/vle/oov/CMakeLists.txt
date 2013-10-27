add_sources(vlelib CairoPlugin.cpp CairoPlugin.hpp Plugin.cpp
  Plugin.hpp StreamReader.cpp StreamReader.hpp)

install(FILES CairoPlugin.hpp Plugin.hpp StreamReader.hpp DESTINATION
  ${VLE_INCLUDE_DIRS}/oov)
