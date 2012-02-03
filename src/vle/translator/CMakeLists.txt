add_sources(vlelib GraphTranslator.cpp GraphTranslator.hpp MatrixTranslator.cpp
  MatrixTranslator.hpp)

install(FILES GraphTranslator.hpp MatrixTranslator.hpp DESTINATION
  ${VLE_INCLUDE_DIRS}/translator)
