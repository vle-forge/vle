add_sources(vlelib CairoPlugin.cpp CairoPlugin.hpp
  LocalStreamReader.cpp LocalStreamReader.hpp NetStreamReader.cpp
  NetStreamReader.hpp OutputMatrix.cpp OutputMatrix.hpp Plugin.cpp
  Plugin.hpp StreamReader.cpp StreamReader.hpp)

install(FILES CairoPlugin.hpp LocalStreamReader.hpp
  NetStreamReader.hpp OutputMatrix.hpp Plugin.hpp StreamReader.hpp
  DESTINATION ${VLE_INCLUDE_DIRS}/oov)
