add_subdirectory(details)

add_sources(vlelib Context.cpp ContextSettings.cpp DateTime.cpp
  DownloadManager.cpp Filesystem.cpp ModuleManager.cpp Package.cpp
  PackageTable.cpp Parser.cpp Rand.cpp RemoteManager.cpp Template.cpp
  Tools.cpp Trace.cpp)

install(FILES Algo.hpp Context.hpp DateTime.hpp Deprecated.hpp
  DownloadManager.hpp Exception.hpp Filesystem.hpp ModuleManager.hpp
  Package.hpp PackageTable.hpp Parser.hpp Rand.hpp RemoteManager.hpp
  Spawn.hpp Template.hpp Tools.hpp Trace.hpp Types.hpp DESTINATION
  ${VLE_INCLUDE_DIRS}/utils)

if (VLE_HAVE_UNITTESTFRAMEWORK)
  add_subdirectory(test)
endif ()
