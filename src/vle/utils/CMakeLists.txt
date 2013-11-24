add_subdirectory(details)

add_sources(vlelib Algo.hpp DateTime.cpp DateTime.hpp Deprecated.hpp
  DownloadManager.cpp DownloadManager.hpp Exception.hpp i18n.hpp
  ModuleManager.cpp ModuleManager.hpp Package.cpp Package.hpp
  PackageTable.cpp PackageTable.hpp Parser.cpp Parser.hpp Path.hpp
  Preferences.cpp Preferences.hpp Rand.cpp Rand.hpp RemoteManager.cpp
  RemoteManager.hpp Spawn.hpp Template.cpp Template.hpp Tools.cpp Tools.hpp
  Trace.cpp Trace.hpp Types.hpp)

install(FILES Algo.hpp DateTime.hpp Deprecated.hpp DownloadManager.hpp
  Exception.hpp i18n.hpp ModuleManager.hpp Package.hpp PackageTable.hpp
  Parser.hpp Path.hpp Preferences.hpp Rand.hpp RemoteManager.hpp
  Spawn.hpp Template.hpp Tools.hpp Trace.hpp Types.hpp DESTINATION
  ${VLE_INCLUDE_DIRS}/utils)

if (VLE_HAVE_UNITTESTFRAMEWORK)
  add_subdirectory(test)
endif ()
