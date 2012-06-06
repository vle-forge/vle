add_sources(vlelib Base.hpp Class.cpp Classes.cpp Classes.hpp
  Class.hpp Condition.cpp Condition.hpp Conditions.cpp Conditions.hpp
  Dynamic.cpp Dynamic.hpp Dynamics.cpp Dynamics.hpp Experiment.cpp
  Experiment.hpp Model.cpp Model.hpp Observable.cpp Observable.hpp
  Observables.cpp Observables.hpp Output.cpp Output.hpp Outputs.cpp
  Outputs.hpp Port.hpp Project.cpp Project.hpp SaxParser.cpp
  SaxParser.hpp SaxStackValue.cpp SaxStackValue.hpp SaxStackVpz.cpp
  SaxStackVpz.hpp Structures.hpp View.cpp View.hpp Views.cpp Views.hpp
  Vpz.cpp Vpz.hpp AtomicModel.cpp AtomicModel.hpp CoupledModel.cpp
  CoupledModel.hpp BaseModel.cpp BaseModel.hpp ModelPortList.cpp
  ModelPortList.hpp)

install(FILES Base.hpp Classes.hpp Class.hpp Condition.hpp
  Conditions.hpp Dynamic.hpp Dynamics.hpp Experiment.hpp Model.hpp
  Observable.hpp Observables.hpp Output.hpp Outputs.hpp Port.hpp
  Project.hpp SaxParser.hpp SaxStackValue.hpp SaxStackVpz.hpp
  Structures.hpp View.hpp Views.hpp Vpz.hpp AtomicModel.hpp
  CoupledModel.hpp BaseModel.hpp ModelPortList.hpp DESTINATION
  ${VLE_INCLUDE_DIRS}/vpz)

if (VLE_HAVE_UNITTESTFRAMEWORK)
  add_subdirectory(test)
endif ()
