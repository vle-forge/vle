add_sources(vlelib ExperimentGenerator.cpp ExperimentGenerator.hpp
  Manager.cpp Manager.hpp Simulation.cpp Simulation.hpp Types.hpp)

install(FILES ExperimentGenerator.hpp Manager.hpp Simulation.hpp
  Types.hpp DESTINATION ${VLE_INCLUDE_DIRS}/manager)

add_subdirectory(test)
