add_sources(vlelib ExperimentGenerator.cpp ExperimentGenerator.hpp
  JustRun.cpp JustRun.hpp Manager.cpp Manager.hpp Run.cpp Run.hpp
  RunManager.cpp RunManager.hpp Types.hpp)

install(FILES ExperimentGenerator.hpp JustRun.hpp Manager.hpp Run.hpp
  RunManager.hpp Types.hpp DESTINATION ${VLE_INCLUDE_DIRS}/manager)

if (HAVE_UNITTESTFRAMEWORK)
  add_subdirectory(test)
endif (HAVE_UNITTESTFRAMEWORK)
