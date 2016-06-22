if (WIN32)
  set (SPECIFIC_UTILS_DETAILS UtilsWin.hpp UtilsWin.cpp
       PathWin32.cpp SpawnWin.cpp)
else ()
  set (SPECIFIC_UTILS_DETAILS PathUnix.cpp SpawnUnix.cpp)
endif ()

add_sources(vlelib Package.hpp PackageManager.hpp PackageManager.cpp
  PackageParser.cpp PackageParser.hpp ${SPECIFIC_UTILS_DETAILS})
