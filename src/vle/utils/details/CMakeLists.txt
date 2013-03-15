if (WIN32)
  set (UTILS_SPECIFIC_SPAWN_IMPL SpawnWin.cpp)
else ()
  set (UTILS_SPECIFIC_SPAWN_IMPL SpawnUnix.cpp)
endif ()

add_sources(vlelib Compress.cpp Package.hpp PackageManager.hpp
  PackageManager.cpp PackageParser.cpp PackageParser.hpp
  ${UTILS_SPECIFIC_SPAWN_IMPL})
