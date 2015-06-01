

add_sources(vlelib Plugin.cpp Plugin.hpp StreamReader.cpp
    StreamReader.hpp)
install(FILES Plugin.hpp StreamReader.hpp DESTINATION
    ${VLE_INCLUDE_DIRS}/oov)

