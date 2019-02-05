install(FILES
  Authors.txt Description.txt License.txt News.txt Readme.txt
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/vle-${VLE_ABI}/pkgs/vle.generic.builder)

Declare(simulator pkg-builder vle.generic.builder Builder src/Builder.cpp)
