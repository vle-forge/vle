add_subdirectory(details)

add_sources(vlelib Context.cpp ContextModule.cpp ContextSettings.cpp
  DateTime.cpp DownloadManager.cpp Exception.cpp Filesystem.cpp
  Package.cpp PackageTable.cpp Parser.cpp Rand.cpp RemoteManager.cpp
  Template.cpp Tools.cpp)

install(FILES Algo.hpp Array.hpp Context.hpp DateTime.hpp
  Deprecated.hpp DownloadManager.hpp Exception.hpp Filesystem.hpp
  Package.hpp PackageTable.hpp Parser.hpp Rand.hpp RemoteManager.hpp
  Spawn.hpp Template.hpp Tools.hpp Types.hpp unit-test.hpp DESTINATION
  ${VLE_INCLUDE_DIRS}/utils)

add_subdirectory(test)
