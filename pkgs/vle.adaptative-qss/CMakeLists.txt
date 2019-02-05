install(FILES
  Authors.txt Description.txt License.txt News.txt Readme.txt
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/vle-${VLE_ABI}/pkgs/vle.adaptative-qss)

Declare(simulator pkg-adder vle.adaptative-qss Adder src/Adder.cpp)
Declare(simulator pkg-generator vle.adaptative-qss Generator src/Generator.cpp)
Declare(simulator pkg-quantifier vle.adaptative-qss AdaptativeQuantifier src/AdaptativeQuantifier.cpp)
Declare(simulator pkg-integrator vle.adaptative-qss Integrator src/Integrator.cpp)
Declare(simulator pkg-mult vle.adaptative-qss Mult src/Mult.cpp)
Declare(simulator pkg-constant vle.adaptative-qss Constant src/Constant.cpp)
Declare(simulator pkg-plot vle.adaptative-qss Plot src/Plot.cpp)
