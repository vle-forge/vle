install(FILES
  Authors.txt Description.txt License.txt News.txt Readme.txt
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/vle-${VLE_ABI}/pkgs/vle.output)

Declare(output pkg-dummy vle.output dummy Dummy.cpp)
Declare(output pkg-file vle.output file "File.cpp;FileType.cpp")
Declare(output pkg-storage vle.output storage Storage.cpp)
Declare(output pkg-console vle.output console Console.cpp)

if (WITH_GVLE)
  add_subdirectory(gvle)
endif()
