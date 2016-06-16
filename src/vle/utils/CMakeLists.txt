add_subdirectory(details)

add_sources(vlelib DateTime.cpp DownloadManager.cpp Filesystem.cpp
  ModuleManager.cpp Package.cpp PackageTable.cpp Parser.cpp
  Preferences.cpp Rand.cpp RemoteManager.cpp Template.cpp Tools.cpp
  Trace.cpp)

install(FILES Algo.hpp DateTime.hpp Deprecated.hpp DownloadManager.hpp
  Exception.hpp ModuleManager.hpp Package.hpp PackageTable.hpp
  Parser.hpp Path.hpp Preferences.hpp Rand.hpp RemoteManager.hpp
  Spawn.hpp Template.hpp Tools.hpp Trace.hpp Types.hpp DESTINATION
  ${VLE_INCLUDE_DIRS}/utils)

if (VLE_HAVE_UNITTESTFRAMEWORK)
  add_subdirectory(test)
endif ()
