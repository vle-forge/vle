add_sources(vlelib AtomicModel.cpp AtomicModel.hpp CoupledModel.cpp
  CoupledModel.hpp Model.cpp Model.hpp ModelPortList.cpp ModelPortList.hpp)

install(FILES AtomicModel.hpp CoupledModel.hpp Model.hpp
  ModelPortList.hpp DESTINATION ${VLE_INCLUDE_DIRS}/graph)

if (HAVE_UNITTESTFRAMEWORK)
  add_subdirectory(test)
endif (HAVE_UNITTESTFRAMEWORK)
