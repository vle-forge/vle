if (WIN32)
  set (SPECIFIC_UTILS_DETAILS UtilsWin.hpp UtilsWin.cpp
       PathWin32.cpp SpawnWin.cpp)
else ()
  set (SPECIFIC_UTILS_DETAILS PathUnix.cpp SpawnUnix.cpp)
endif ()

add_sources(vlelib Compress.cpp Package.hpp PackageManager.hpp
  PackageManager.cpp PackageParser.cpp PackageParser.hpp Path.cpp
  ${SPECIFIC_UTILS_DETAILS})
